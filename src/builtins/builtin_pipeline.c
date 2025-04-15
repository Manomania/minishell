/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_pipeline.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 10:20:14 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/15 18:03:27 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "debug.h"
#include "error.h"
#include "minishell.h"

/**
 * Checks if a command is a built-in
 *
 * @param cmd_name Command name to check
 * @return t_bool true if built-in, false otherwise
 */
t_bool	is_builtin_command(char *cmd_name)
{
	if (!cmd_name)
		return (0);
	if (ft_strncmp(cmd_name, "exit", __INT_MAX__) == 0 || ft_strncmp(cmd_name,
			"echo", __INT_MAX__) == 0 || ft_strncmp(cmd_name, "cd",
			__INT_MAX__) == 0 || ft_strncmp(cmd_name, "pwd", __INT_MAX__) == 0
		|| ft_strncmp(cmd_name, "export", __INT_MAX__) == 0
		|| ft_strncmp(cmd_name, "unset", __INT_MAX__) == 0
		|| ft_strncmp(cmd_name, "env", __INT_MAX__) == 0)
		return (1);
	return (0);
}

/**
 * Preprocess redirections for a command in pipeline
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
			if (fd != -1)
			{
				if (redir->type == TOK_REDIR_TO)
					write(fd, "", 0);
				close(fd);
			}
		}
		redir = redir->next;
	}
	return (0);
}

/**
 * Sets up and redirects file descriptors for pipeline execution
 *
 * @param saved_in Pointer to store saved stdin
 * @param saved_out Pointer to store saved stdout
 * @param input_fd Input file descriptor
 * @param output_fd Output file descriptor
 * @return Status code (0 for success, error code otherwise)
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
			if (*input_fd > 2)
				close(*input_fd);
			restore_pipeline_fds(*saved_in, *saved_out);
			return (ERR_FD);
		}
		if (*input_fd > 2)
		{
			close(*input_fd);
			*input_fd = -1;
		}
	}
	if (*output_fd != STDOUT_FILENO)
	{
		if (dup2(*output_fd, STDOUT_FILENO) == -1)
		{
			if (*output_fd > 2)
				close(*output_fd);
			restore_pipeline_fds(*saved_in, *saved_out);
			return (ERR_FD);
		}
		if (*output_fd > 2)
		{
			close(*output_fd);
			*output_fd = -1;
		}
	}
	return (0);
}

/**
 * Executes the appropriate built-in command
 *
 * @param ctx Context containing environment
 * @param cmd Command to execute
 * @return Exit status of the built-in
 */
static int	execute_builtin_command(t_ctx *ctx, t_command *cmd)
{
	int	exit_status;

	if (ft_strncmp(cmd->args[0], "exit", __INT_MAX__) == 0)
	{
		exit_status = builtin_exit(ctx, cmd);
		ctx->exit_requested = 0;
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
 * Executes a built-in command in a pipeline
 *
 * @param ctx Context for shell environment
 * @param cmd Command to execute
 * @param input_fd Input file descriptor
 * @param output_fd Output file descriptor
 * @return Exit status of the built-in
 */
static int	execute_pipeline_builtin(t_ctx *ctx, t_command *cmd, int *input_fd,
		int *output_fd)
{
	int	saved_in;
	int	saved_out;
	int	fd_status;
	int	exit_status;
	int	builtin_saved_fds[2];

	builtin_saved_fds[0] = -1;
	builtin_saved_fds[1] = -1;
	fd_status = setup_pipeline_fds(&saved_in, &saved_out, input_fd, output_fd);
	if (fd_status != 0)
		return (error(NULL, "pipeline", fd_status));
	if (setup_builtin_redirections(cmd, builtin_saved_fds) != 0)
	{
		exit_status = 1;
	}
	else
	{
		exit_status = execute_builtin_command(ctx, cmd);
	}
	restore_pipeline_fds(saved_in, saved_out);
	builtin_restore_redirections(builtin_saved_fds);
	return (exit_status);
}

/**
 * Executes a command in a pipeline context
 *
 * @param ctx Context for shell environment
 * @param cmd Command to execute
 * @param input_fd Input file descriptor
 * @param output_fd Output file descriptor
 * @return Process ID or -2 for built-in execution
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
