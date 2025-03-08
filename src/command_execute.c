/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_execute.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 16:10:59 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/08 18:00:19 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Handles the execution of a command in a pipeline
 *
 * @param ctx Context
 * @param cmd Command to execute
 * @param pipes Array of pipe file descriptors
 * @param cmd_index Index of the command in the pipeline
 * @param cmd_count Total number of commands in the pipeline
 * @return void Function doesn't return if successful
 */
void	execute_command_in_pipeline(t_ctx *ctx, t_command *cmd, int pipes[2][2],
		int cmd_index, int cmd_count)
{
	reset_signals();
	if (cmd_index > 0)
		dup2(pipes[0][0], STDIN_FILENO);
	if (cmd_index < cmd_count - 1)
		dup2(pipes[1][1], STDOUT_FILENO);
	close_all_pipes(pipes);
	if (builtins_try(ctx, cmd))
		exit(0);
	command_execute(ctx, cmd);
	exit(1);
}

/**
 * @brief Executes a command with execve
 *
 * @param ctx Context containing environment
 * @param cmd Command to execute
 * @return int 0 on success, -1 on error
 */
int	command_execute(t_ctx *ctx, t_command *cmd)
{
	char	*bin_path;

	if (!cmd->cmd)
		return (-1);
	if (builtins_try(ctx, cmd))
		return (0);
	bin_path = bin_find(ctx, cmd->cmd);
	if (!bin_path)
		return (ctx_error(ERR_CMD_NOT_FOUND));
	if (handle_redirections(cmd->redirections) != 0)
		return (-1);
	if (execve(bin_path, cmd->args, ctx->envp) == -1)
		ctx_error(ERR_CMD_NOT_FOUND);
	return (-1);
}
