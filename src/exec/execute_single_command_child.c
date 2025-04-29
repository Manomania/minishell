/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_single_command_child.c                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 15:18:36 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/29 15:20:42 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
		cleanup_child_process(ctx);
		return (1);
	}
	return (0);
}

/**
 * @brief Handle command binary resolution and errors
 *
 * @param ctx Context containing environment and command info
 * @return int 0 if command is not found or invalid, 1 on success
 */
static int	ecp_handle_command_binary(t_ctx *ctx)
{
	if (!command_bin(ctx))
	{
		if (ctx->cmd->args && ctx->cmd->args[0])
		{
			if (ft_strchr(ctx->cmd->args[0], '/'))
				exit(126);
			else
				exit(127);
		}
		cleanup_child_process(ctx);
		return (0);
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
		cleanup_child_process(ctx);
		exit(1);
	}
	execve(ctx->cmd->args[0], ctx->cmd->args, env_array);
	perror("execve");
	free_2d_array((void **)env_array);
	cleanup_child_process(ctx);
	exit(126);
}

/**
 * @brief Function executed by child process
 *
 * @param ctx Context containing environment and command info
 * @param saved_stdin Saved standard input descriptor
 * @param saved_stdout Saved standard output descriptor
 */
void	execute_child_process(t_ctx *ctx, int saved_stdin, int saved_stdout)
{
	int	result;
	int	cmd_valid;

	ecp_close_saved_fds(saved_stdin, saved_stdout);
	result = ecp_prepare_command_io(ctx);
	if (result != 0)
		exit(1);
	cmd_valid = ecp_handle_command_binary(ctx);
	if (!cmd_valid)
		exit(127);
	ecp_run_command_with_execve(ctx);
}
