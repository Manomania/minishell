/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_execute.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 16:10:59 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/29 12:12:18 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"

/**
 * @brief Checks if the command context is valid for execution
 *
 * @param ctx Context containing environment and command info
 * @return int 1 if valid, 0 if nothing to execute
 */
static int	validate_command_context(t_ctx *ctx)
{
	if (!ctx || !ctx->cmd)
	{
		error(NULL, "validate_command_context", ERR_ALLOC);
		ctx->exit_status = 1;
		return (0);
	}
	if (!ctx->cmd->args && !ctx->cmd->redirection)
	{
		ctx->exit_status = 0;
		return (0);
	}
	return (1);
}

/**
 * @brief Checks if the command is part of a pipeline
 *
 * @param cmd Command to check
 * @return t_bool true if command is part of pipeline, false otherwise
 */
static t_bool	is_pipeline(t_command *cmd)
{
	return (cmd && cmd->next);
}

/**
 * @brief Main execution entry point for a parsed command line
 * Handles heredocs, validates context, and dispatches execution
 *
 * @param ctx Context containing environment and command info
 * @return int Exit status of the command/pipeline
 */
int	command_execute(t_ctx *ctx)
{
	int	status;

	if (!validate_command_context(ctx))
		return (ctx->exit_status);
	if (read_all_heredocs(ctx) != 0)
	{
		cleanup_heredoc_resources(ctx);
		return (ctx->exit_status);
	}
	if (is_pipeline(ctx->cmd))
		status = execute_pipeline(ctx);
	else if (has_only_redirections(ctx->cmd))
		status = execute_redirections_only(ctx);
	else
		status = execute_single_command(ctx);
	ctx->exit_status = status;
	cleanup_heredoc_resources(ctx);
	return (status);
}
