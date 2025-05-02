/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipeline_process_command.c                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 15:06:15 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/02 13:58:50 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "error.h"
#include "minishell.h"

/**
 * @brief Handle execution of builtin commands in the pipeline
 *
 * @param ctx Context containing shell state
 * @param a Arguments structure containing command details
 * @param is_last Flag indicating if this is the last command
 * @param output_fd Output file descriptor
 * @return int 0 on success
 */
static int	eppc_handle_pipeline_builtin(t_ctx *ctx, t_ppca a, int is_last,
		int output_fd)
{
	ctx->exit_status = execute_pipeline_builtin(ctx, a.cmd, a.prev_pipe,
			output_fd);
	a.pids[a.cmd_index] = -2;
	if (a.prev_pipe != STDIN_FILENO)
		close(a.prev_pipe);
	if (!is_last && a.pipe_fds[1] != STDOUT_FILENO)
		close(a.pipe_fds[1]);
	return (0);
}

/**
 * @brief Fork a child process to execute a command
 *
 * @param ctx Context containing shell state
 * @param a Arguments structure containing command details
 * @param is_last Flag indicating if this is the last command
 * @param output_fd Output file descriptor
 * @return int 0 on success, -1 on error
 */
static int	eppc_fork_pipeline_command(t_ctx *ctx, t_ppca a, int is_last,
		int output_fd)
{
	pid_t	pid;

	setup_parent_signals();
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		if (a.prev_pipe != STDIN_FILENO)
			close(a.prev_pipe);
		if (!is_last)
		{
			close(a.pipe_fds[0]);
			close(a.pipe_fds[1]);
		}
		return (-1);
	}
	if (pid == 0)
	{
		if (!is_last)
			close(a.pipe_fds[0]);
		execute_pipeline_command(ctx, a.cmd, a.prev_pipe, output_fd);
		exit(EXIT_FAILURE);
	}
	a.pids[a.cmd_index] = pid;
	return (0);
}

/**
 * @brief Close file descriptors after command execution
 *
 * @param a Arguments structure containing command details
 * @param is_last Flag indicating if this is the last command
 */
static void	eppc_cleanup_pipeline_fds(t_ppca a, int is_last)
{
	if (a.prev_pipe != STDIN_FILENO)
		close(a.prev_pipe);
	if (!is_last && a.pipe_fds[1] != STDOUT_FILENO)
		close(a.pipe_fds[1]);
}

/**
 * @brief Handle empty command error case
 *
 * @param ctx Context containing shell state
 * @param a Arguments structure containing command details
 * @param is_last Flag indicating if this is the last command
 * @return int 0 on success
 */
static int	handle_empty_command(t_ctx *ctx, t_ppca a, int is_last)
{
	ft_printf_fd(STDERR_FILENO, "minishell: : command not found\n");
	ctx->exit_status = 127;
	a.pids[a.cmd_index] = -2;
	eppc_cleanup_pipeline_fds(a, is_last);
	return (0);
}

/**
 * @brief Process one command in the pipeline
 *
 * @param ctx Context containing shell state
 * @param a Arguments structure containing command details
 * @return int 0 on success, -1 on error
 */
int	process_pipeline_command(t_ctx *ctx, t_ppca a)
{
	int	is_last;
	int	output_fd;
	int	result;

	is_last = (a.cmd->next == NULL);
	output_fd = STDOUT_FILENO;
	if (!is_last)
		output_fd = a.pipe_fds[1];
	if (!a.cmd->args || !a.cmd->args[0] || a.cmd->args[0][0] == '\0')
		return (handle_empty_command(ctx, a, is_last));
	if (is_path(a.cmd->args[0]) && is_directory(a.cmd->args[0]))
	{
		error(a.cmd->args[0], NULL, ERR_IS_DIR);
		a.pids[a.cmd_index] = -2;
		eppc_cleanup_pipeline_fds(a, is_last);
		ctx->exit_status = 126;
		return (0);
	}
	if (is_builtin_command(a.cmd->args[0]))
		return (eppc_handle_pipeline_builtin(ctx, a, is_last, output_fd));
	result = eppc_fork_pipeline_command(ctx, a, is_last, output_fd);
	if (result == 0)
		eppc_cleanup_pipeline_fds(a, is_last);
	return (result);
}
