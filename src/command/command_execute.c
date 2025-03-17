/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_execute.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 16:10:59 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/17 18:09:14 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "debug.h"
#include "error.h"
#include "minishell.h"

/**
 * @brief Executes a single command without a pipeline
 *
 * This function executes a single command by forking a child process.
 *
 * @param ctx Context containing environment and command info
 * @return Exit status of the command
 */
static int	execute_single_command(t_ctx *ctx)
{
	pid_t	pid;
	int		status;

	if (builtins_try(ctx, ctx->cmd))
		return (0);
	if (!command_bin(ctx))
		return (error_code(ERR_CMD_NOT_FOUND));
	setup_parent_signals();
	pid = fork();
	if (pid == -1)
		return (ctx_error(ERR_CHILD));
	if (pid == 0)
		execute_child(ctx);
	waitpid(pid, &status, 0);
	setup_signals();
	return (get_exit_status(status));
}

/**
 * @brief Checks if the command is part of a pipeline
 *
 * This function determines if a command is part of a pipeline.
 *
 * @param cmd Command to check
 * @return true if command is part of pipeline, false otherwise
 */
static t_bool	is_pipeline(t_command *cmd)
{
	return (cmd && cmd->next);
}

/**
 * @brief Executes a command or pipeline
 *
 * @param ctx Context containing environment and command info
 * @return Exit status of the command
 */
int	command_execute(t_ctx *ctx)
{
	if (!ctx || !ctx->cmd)
		return (ctx_error(ERR_ALLOC));
	if (!ctx->cmd->args || !ctx->cmd->args[0])
		return (-1);
	debug_log(DEBUG_INFO, "execute", "Executing command");
	if (is_pipeline(ctx->cmd))
	{
		debug_log(DEBUG_INFO, "execute", "Pipeline detected");
		return (exec_cmdas(ctx));
	}
	return (execute_single_command(ctx));
}
