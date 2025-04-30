/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_single_command_child.c                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 15:18:36 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/30 12:39:55 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"

/**
 * @brief Close saved file descriptors
 *
 * @param saved_stdin Saved standard input descriptor
 * @param saved_stdout Saved standard output descriptor
 */
static void	ecp_close_saved_fds(int saved_stdin, int saved_stdout)
{
	if (saved_stdin != -1)
		close(saved_stdin);
	if (saved_stdout != -1)
		close(saved_stdout);
}

/**
 * @brief Set up redirections and check for errors
 *
 * @param ctx Context containing environment and command info
 * @return int 0 on success, non-zero on error
 */
static int	ecp_prepare_command_io(t_ctx *ctx)
{
	int	result;

	reset_signals();
	result = setup_command_redirections(ctx, ctx->cmd);
	if (result != 0)
	{
		ctx->exit_status = 1;
		cleanup_child_process(ctx);
		return (ctx->exit_status);
	}
	return (0);
}

/**
 * @brief Check if the command is a directory and handle appropriately
 *
 * @param ctx Context containing environment and command info
 * @return int 0 if not a directory, 126 if it is
 */
static int	ecp_check_directory(t_ctx *ctx)
{
	if (is_path(ctx->cmd->args[0]) && is_directory(ctx->cmd->args[0]))
	{
		ctx->exit_status = error(ctx->cmd->args[0], NULL, ERR_IS_DIR);
		cleanup_child_process(ctx);
		exit(126);
	}
	return (0);
}

/**
 * @brief Handle command binary resolution and prepare for execve.
 * If command_bin fails, cleanup and exit with the status it set.
 *
 * @param ctx Context containing environment and command info
 * @return int 1 on success (binary found), exits on failure.
 */
static int	ecp_handle_command_binary(t_ctx *ctx)
{
	int	status;

	ecp_check_directory(ctx);
	if (!command_bin(ctx))
	{
		status = ctx->exit_status;
		cleanup_child_process(ctx);
		exit(status);
	}
	return (1);
}

/**
 * @brief Execute the command using execve
 *
 * @param ctx Context containing environment and command info
 */
static void	ecp_run_command_with_execve(t_ctx *ctx)
{
	char	**env_array;

	env_array = create_env_array(ctx->env_list);
	if (!env_array)
	{
		error(NULL, "execve", ERR_ALLOC);
		cleanup_child_process(ctx);
		exit(1);
	}
	execve(ctx->cmd->args[0], ctx->cmd->args, env_array);
	error(ctx->cmd->args[0], "execve", ERR_CMD_NOT_FOUND);
	free_2d_array((void **)env_array);
	cleanup_child_process(ctx);
	if (errno == EACCES)
		exit(126);
	exit(127);
}

/**
 * @brief Function executed by child process for a single external command.
 * Handles IO setup, binary finding, cleanup, and execution/exit.
 *
 * @param ctx Context containing environment and command info
 * @param saved_stdin Saved standard input descriptor (from parent)
 * @param saved_stdout Saved standard output descriptor (from parent)
 */
void	execute_child_process(t_ctx *ctx, int saved_stdin, int saved_stdout)
{
	int	io_status;

	ecp_close_saved_fds(saved_stdin, saved_stdout);
	io_status = ecp_prepare_command_io(ctx);
	if (io_status != 0)
		exit(io_status);
	ecp_handle_command_binary(ctx);
	ecp_run_command_with_execve(ctx);
}
