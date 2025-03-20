/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_execute.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 16:10:59 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/19 18:37:17 by elagouch         ###   ########.fr       */
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

	if (!command_bin(ctx))
	{
		if (ctx->cmd->args && ctx->cmd->args[0] && (ft_strchr(ctx->cmd->args[0],
					'/') || ctx->cmd->args[0][0] == '.'))
			return (2);
		return (error_code(ERR_CMD_NOT_FOUND));
	}
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
	int	status;

	if (!ctx || !ctx->cmd)
	{
		status = ctx_error(ERR_ALLOC);
		ctx->exit_status = status;
		return (status);
	}
	if (!ctx->cmd->args || !ctx->cmd->args[0])
	{
		ctx->exit_status = -1;
		return (-1);
	}
	debug_log(DEBUG_INFO, "execute", "Executing command");
	if (builtins_try(ctx, ctx->cmd))
		return (ctx->exit_status);
	if (is_pipeline(ctx->cmd))
	{
		debug_log(DEBUG_INFO, "execute", "Pipeline detected");
		status = exec_cmdas(ctx);
	}
	else
		status = execute_single_command(ctx);
	ctx->exit_status = status;
	return (status);
}
