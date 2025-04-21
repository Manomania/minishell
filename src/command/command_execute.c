/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_execute.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 16:10:59 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/21 18:03:52 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"

#include "error.h"
#include "minishell.h"

/**
 * @brief Sets up redirections for a command, stopping on the first error.
 * Also handles pre-reading heredocs if not already done.
 *
 * @param ctx Context containing environment info
 * @param cmd Command containing redirections to set up
 * @return int 0 on success, 1 on error (generic redirection failure)
 */
int	setup_command_redirections(t_ctx *ctx, t_command *cmd)
{
	if (setup_heredocs(ctx, cmd) != 0)
		return (1);
	if (setup_redirections(cmd->redirection) != 0)
		return (1);
	return (0);
}

/**
 * @brief Handles command validation and error reporting in child process
 *
 * @param ctx Context containing environment info
 * @return int Exit code for the child process (0 on success, error code
 * otherwise)
 */
int	validate_cmd_in_child(t_ctx *ctx)
{
	if (!ctx->cmd->args || !ctx->cmd->args[0])
		return (EXIT_SUCCESS);
	if (!command_bin(ctx))
	{
		if (ctx->cmd->args && ctx->cmd->args[0])
		{
			if (ft_strchr(ctx->cmd->args[0], '/'))
				return (126);
			else
				return (127);
		}
		return (127);
	}
	return (0);
}

/**
 * @brief Validates if the command context is valid for execution
 *
 * @param ctx Context containing environment and command info
 * @param status Pointer to store error status (unused here, handled by return)
 * @return int 1 if valid, 0 if invalid or nothing to execute
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
 * @brief Main execution entry point for a parsed command line.
 * Handles heredocs, validates context, and dispatches execution.
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
		close_heredoc_fds(ctx->cmd);
		ctx->exit_status = 1;
		return (ctx->exit_status);
	}
	status = process_command_type(ctx);
	close_heredoc_fds(ctx->cmd);
	ctx->exit_status = status;
	return (status);
}
