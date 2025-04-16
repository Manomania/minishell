/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_pipeline.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 10:20:14 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/16 17:07:38 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "debug.h"
#include "error.h"
#include "minishell.h"

/**
 * @brief Sets up stdin/stdout for a built-in command in a pipeline.
 *
 * Saves original std fds and redirects stdin/stdout to input_fd/output_fd.
 * Does NOT close the original pipe FDs (*input_fd, *output_fd); this is
 * handled by the main pipeline loop to prevent double-closes.
 *
 * @param saved_in Pointer to store duplicated original stdin fd.
 * @param saved_out Pointer to store duplicated original stdout fd.
 * @param input_fd Pointer to the fd to use for stdin.
 * @param output_fd Pointer to the fd to use for stdout.
 * @return 0 on success, ERR_IO or ERR_FD on failure.
 */
static int	setup_pipeline_fds(int *saved_in, int *saved_out, int *input_fd,
		int *output_fd)
{
	*saved_in = dup(STDIN_FILENO);
	*saved_out = dup(STDOUT_FILENO);
	if (*saved_in == -1 || *saved_out == -1)
	{
		if (*saved_in != -1)
			close(*saved_in);
		if (*saved_out != -1)
			close(*saved_out);
		return (ERR_IO);
	}
	if (*input_fd != STDIN_FILENO)
	{
		if (dup2(*input_fd, STDIN_FILENO) == -1)
		{
			restore_pipeline_fds(*saved_in, *saved_out);
			return (ERR_FD);
		}
	}
	if (*output_fd != STDOUT_FILENO)
	{
		if (dup2(*output_fd, STDOUT_FILENO) == -1)
		{
			restore_pipeline_fds(*saved_in, *saved_out);
			return (ERR_FD);
		}
	}
	return (0);
}

/**
 * @brief Checks if a command is a built-in
 *
 * @param cmd_name Command name to check
 * @return t_bool true if built-in, false otherwise
 */
t_bool	is_builtin_command(char *cmd_name)
{
	if (!cmd_name)
		return (false);
	if (ft_strncmp(cmd_name, "exit", __INT_MAX__) == 0 || ft_strncmp(cmd_name,
			"echo", __INT_MAX__) == 0 || ft_strncmp(cmd_name, "cd",
			__INT_MAX__) == 0 || ft_strncmp(cmd_name, "pwd", __INT_MAX__) == 0
		|| ft_strncmp(cmd_name, "export", __INT_MAX__) == 0
		|| ft_strncmp(cmd_name, "unset", __INT_MAX__) == 0
		|| ft_strncmp(cmd_name, "env", __INT_MAX__) == 0)
		return (true);
	return (false);
}

/**
 * Preprocess redirections for a command in pipeline (e.g., create output files)
 * This ensures files exist even if the command itself fails later.
 *
 * @param cmd Command with redirections
 * @return 0 on success, -1 on error
 */
static int	preprocess_redirections(t_command *cmd)
{
	t_redirection	*redir;
	int				fd;

	redir = cmd->redirection;
	while (redir)
	{
		if (redir->type == TOK_REDIR_TO || redir->type == TOK_HERE_DOC_TO)
		{
			fd = open_redirection_file(redir);
			if (fd == -1)
				return (-1);
			else
				close(fd);
		}
		redir = redir->next;
	}
	return (0);
}

/**
 * Executes the specific built-in command function based on the command name.
 *
 * @param ctx Context for shell environment (potentially temporary).
 * @param cmd Command to execute.
 * @return int Exit status of the built-in command.
 */
static int	execute_builtin_command(t_ctx *ctx, t_command *cmd)
{
	int	exit_status;

	if (ft_strncmp(cmd->args[0], "exit", __INT_MAX__) == 0)
	{
		exit_status = builtin_exit(ctx, cmd);
		ctx->exit_requested = false;
	}
	else if (ft_strncmp(cmd->args[0], "echo", __INT_MAX__) == 0)
		exit_status = builtin_echo(ctx, cmd);
	else if (ft_strncmp(cmd->args[0], "cd", __INT_MAX__) == 0)
		exit_status = builtin_cd(ctx, cmd);
	else if (ft_strncmp(cmd->args[0], "pwd", __INT_MAX__) == 0)
		exit_status = builtin_pwd(ctx, cmd);
	else if (ft_strncmp(cmd->args[0], "export", __INT_MAX__) == 0)
		exit_status = builtin_export(ctx, cmd);
	else if (ft_strncmp(cmd->args[0], "unset", __INT_MAX__) == 0)
		exit_status = builtin_unset(ctx, cmd);
	else if (ft_strncmp(cmd->args[0], "env", __INT_MAX__) == 0)
		exit_status = builtin_env(ctx, cmd);
	else
		exit_status = 127;
	return (exit_status);
}

/**
 * @brief Executes a built-in command within a pipeline stage.
 *
 * @param ctx The main shell context (read-only for environment here).
 * @param cmd The command structure containing the built-in and args.
 * @param input_fd Pointer to the input file descriptor for this stage.
 * @param output_fd Pointer to the output file descriptor for this stage.
 * @return Exit status of the built-in command.
 */
static int	execute_pipeline_builtin(t_ctx *ctx, t_command *cmd, int *input_fd,
		int *output_fd)
{
	int		saved_stdin;
	int		saved_stdout;
	int		fd_setup_status;
	int		exit_status;
	int		builtin_redir_fds[2];
	t_ctx	temp_ctx;
	t_env	*temp_env_list;

	builtin_redir_fds[0] = -1;
	builtin_redir_fds[1] = -1;
	temp_env_list = duplicate_env_list(ctx->env_list);
	if (!temp_env_list)
	{
		error(NULL, "pipeline_builtin", ERR_ALLOC);
		return (1);
	}
	temp_ctx = *ctx;
	temp_ctx.env_list = temp_env_list;
	fd_setup_status = setup_pipeline_fds(&saved_stdin, &saved_stdout, input_fd,
			output_fd);
	if (fd_setup_status != 0)
	{
		free_env_list(temp_env_list);
		return (error(NULL, "pipeline_builtin", fd_setup_status));
	}
	if (setup_builtin_redirections(cmd, builtin_redir_fds) != 0)
		exit_status = 1;
	else
		exit_status = execute_builtin_command(&temp_ctx, cmd);
	restore_pipeline_fds(saved_stdin, saved_stdout);
	builtin_restore_redirections(builtin_redir_fds);
	free_env_list(temp_env_list);
	return (exit_status);
}

/**
 * Executes a command (either built-in or external) in a pipeline context.
 * For external commands, it forks a child process.
 * For built-in commands, it executes them directly but isolates environment
 * changes.
 *
 * @param ctx Context for shell environment.
 * @param cmd Command to execute.
 * @param input_fd Pointer to the input file descriptor for this stage.
 * @param output_fd Pointer to the output file descriptor for this stage.
 * @return pid_t Process ID of the child if forked, -2 if a built-in was
 * executed directly, -1 on error.
 */
pid_t	execute_pipeline_command(t_ctx *ctx, t_command *cmd, int *input_fd,
		int *output_fd)
{
	pid_t	pid;
	int		status;

	if (!cmd->args || !cmd->args[0])
		return (-1);
	preprocess_redirections(cmd);
	if (is_builtin_command(cmd->args[0]))
	{
		status = execute_pipeline_builtin(ctx, cmd, input_fd, output_fd);
		ctx->exit_status = status;
		return (-2);
	}
	else
	{
		pid = fork();
		if (pid == -1)
		{
			perror("fork");
			return (-1);
		}
		if (pid == 0)
			setup_child_process(ctx, cmd, *input_fd, *output_fd);
		return (pid);
	}
}
