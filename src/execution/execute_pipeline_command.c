/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipeline_command.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 15:48:56 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/05 15:50:40 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "execute.h"
#include "minishell.h"

/**
 * @brief Execute external command with execve
 *
 * Finds binary path and executes external command
 *
 * @param ctx Shell context
 * @param cmd Command to execute
 * @param pids Process ids pointer to free
 */
void	execute_external_command(t_ctx *ctx, t_command *cmd, int *pids)
{
	char	*bin_path;

	reset_signals();
	bin_path = bin_find(ctx, cmd->args[0]);
	if (!bin_path)
		child_process_exit(ctx, pids, ctx->exit_status);
	execve(bin_path, cmd->args, ctx->envp);
	free(bin_path);
	child_process_exit(ctx, pids, error(cmd->args[0], NULL, ERR_CMD_NOT_FOUND));
}

/**
 * @brief Executes a command in a child process
 *
 * Handles redirections and executes the command
 *
 * @param ctx Shell context
 * @param cmd Command to execute
 * @param fds Input and output file descriptors
 * @param pids Process ids pointer to free
 */
void	execute_command_in_child(t_ctx *ctx, t_command *cmd, t_fds fds,
		int *pids)
{
	int	status;

	handle_fd_redirection(fds);
	if (!apply_redirections(cmd))
		child_process_exit(ctx, pids, 1);
	if (is_builtin_command(cmd->args[0]))
	{
		status = execute_builtin(ctx, cmd);
		child_process_exit(ctx, pids, status);
	}
	else
		execute_external_command(ctx, cmd, pids);
}
