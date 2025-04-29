/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipeline_utils.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 15:52:23 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/29 13:40:50 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "error.h"
#include "minishell.h"

/**
 * @brief Counts the number of commands in a pipeline
 *
 * @param cmd The first command in the pipeline
 * @return int The number of commands
 */
int	count_commands(t_command *cmd)
{
	int			count;
	t_command	*current;

	count = 0;
	current = cmd;
	while (current)
	{
		count++;
		current = current->next;
	}
	return (count);
}

/**
 * @brief Process one command in the pipeline
 *
 * @param ctx Context containing shell state
 * @param cmd Current command to process
 * @param prev_pipe Previous pipe read end
 * @param pipe_fds Current pipe file descriptors
 * @param pids Array of process IDs
 * @param cmd_index Current command index
 * @return int 0 on success, -1 on error
 */
int	process_pipeline_command(t_ctx *ctx, t_command *cmd, int prev_pipe,
		int pipe_fds[2], pid_t *pids, int cmd_index)
{
	int		is_last;
	int		output_fd;
	pid_t	pid;

	is_last = (cmd->next == NULL);
	output_fd = is_last ? STDOUT_FILENO : pipe_fds[1];
	if (is_builtin_command(cmd->args[0]))
	{
		ctx->exit_status = execute_pipeline_builtin(ctx, cmd, prev_pipe,
				output_fd);
		pids[cmd_index] = -2;
		if (prev_pipe != STDIN_FILENO)
			close(prev_pipe);
		if (!is_last && pipe_fds[1] != STDOUT_FILENO)
			close(pipe_fds[1]);
		return (0);
	}
	setup_parent_signals();
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		if (prev_pipe != STDIN_FILENO)
			close(prev_pipe);
		if (!is_last)
		{
			close(pipe_fds[0]);
			close(pipe_fds[1]);
		}
		return (-1);
	}
	if (pid == 0)
	{
		if (!is_last)
			close(pipe_fds[0]);
		execute_pipeline_command(ctx, cmd, prev_pipe, output_fd);
		exit(EXIT_FAILURE);
	}
	pids[cmd_index] = pid;
	if (prev_pipe != STDIN_FILENO)
		close(prev_pipe);
	if (!is_last && pipe_fds[1] != STDOUT_FILENO)
		close(pipe_fds[1]);
	return (0);
}

/**
 * @brief Handles command execution after redirection setup
 *
 * @param ctx Context with environment
 * @param cmd Command to execute
 */
void	execute_command(t_ctx *ctx, t_command *cmd)
{
	char	*bin_path;

	if (builtins_try(ctx, cmd))
	{
		cleanup_child_process(ctx);
		exit(EXIT_SUCCESS);
	}
	bin_path = validate_and_resolve_command(ctx, cmd);
	if (!bin_path && (!cmd->args || !cmd->args[0]))
	{
		cleanup_child_process(ctx);
		exit(EXIT_SUCCESS);
	}
	else if (!bin_path)
	{
		cleanup_child_process(ctx);
		exit(127);
	}
	free(cmd->args[0]);
	cmd->args[0] = bin_path;
	if (execve(cmd->args[0], cmd->args, ctx->envp) == -1)
		ctx_error_exit(ctx, cmd->args[0], "exec", ERR_CHILD);
}

/**
 * Handles redirections and pipe setup for pipeline commands
 *
 * @param cmd Command to process
 * @param input_fd Input file descriptor
 * @param output_fd Output file descriptor
 * @return 0 on success, -1 on error
 */
int	setup_child_pipeline_redirections(t_command *cmd, int input_fd,
		int output_fd)
{
	if (input_fd != STDIN_FILENO)
	{
		if (dup2(input_fd, STDIN_FILENO) == -1)
		{
			close(input_fd);
			return (-1);
		}
		close(input_fd);
	}
	if (output_fd != STDOUT_FILENO)
	{
		if (dup2(output_fd, STDOUT_FILENO) == -1)
		{
			close(output_fd);
			return (-1);
		}
		close(output_fd);
	}
	return (apply_child_redirections(cmd));
}

/**
 * @brief Clean up resources before child process exit
 *
 * @param ctx Context containing all resources
 */
void	cleanup_child_process(t_ctx *ctx)
{
	cleanup_heredoc_resources(ctx);
	if (ctx->tokens)
	{
		free_all_token(ctx->tokens);
		ctx->tokens = NULL;
	}
	ctx_clear(ctx);
}

/**
 * @brief Validates command and resolves binary path
 *
 * @param ctx Context with environment
 * @param cmd Command to validate
 * @return char* The resolved binary path or NULL
 */
char	*validate_and_resolve_command(t_ctx *ctx, t_command *cmd)
{
	char	*bin_path;

	if (!cmd->args || !cmd->args[0])
		return (NULL);
	bin_path = bin_find(ctx, cmd->args[0]);
	if (!bin_path)
	{
		if (cmd->args[0][0] == '.' && !ft_strchr(cmd->args[0], '/'))
			ctx_error_exit(ctx, NULL, "exec", ERR_NO_FILE);
		if (ft_strchr(cmd->args[0], '/'))
			ctx_error_exit(ctx, cmd->args[0], "exec", ERR_IS_DIR);
		ft_printf_fd(STDERR_FILENO, "minishell: %s: command not found\n",
			cmd->args[0]);
	}
	return (bin_path);
}

/**
 * Handles here-doc redirection in a child process
 *
 * @param redir Redirection to handle
 * @return 0 on success, -1 on error
 */
static int	apply_heredoc_redirection(t_redirection *redir)
{
	int	result;

	if (redir->fd >= 0)
	{
		result = dup2(redir->fd, STDIN_FILENO);
		close(redir->fd);
		redir->fd = -1;
		if (result == -1)
			return (-1);
	}
	return (0);
}

/**
 * Applies redirections for a specific command in a child process
 *
 * @param cmd Command containing redirections
 * @return 0 on success, -1 on error
 */
int	apply_child_redirections(t_command *cmd)
{
	t_redirection	*redir;
	int				result;

	if (!cmd || !cmd->redirection)
		return (0);
	redir = cmd->redirection;
	while (redir)
	{
		if (redir->type == TOK_HERE_DOC_FROM)
			result = apply_heredoc_redirection(redir);
		else
			result = apply_redirection(redir);
		if (result == -1)
			return (-1);
		redir = redir->next;
	}
	return (0);
}
