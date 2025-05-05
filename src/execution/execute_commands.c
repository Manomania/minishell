/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_commands.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 16:44:19 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/05 14:34:36 by maximart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "error.h"
#include "execute.h"

/**
 * @brief Executes special cases of commands
 *
 * @param ctx Shell context
 * @param cmd Command to execute
 */
static t_bool	special_cases(t_ctx *ctx, t_command *cmd)
{
	if (cmd->arg_count == 0 && cmd->next == NULL && cmd->args[0]
		&& ft_strncmp(cmd->args[0], ":", 3) == 0)
		return (ctx->exit_status = 0, true);
	if (cmd->arg_count == 0 && cmd->next == NULL && cmd->args[0]
		&& ft_strncmp(cmd->args[0], "!", 3) == 0)
		return (ctx->exit_status = 1, true);
	return (false);
}

/**
 * @brief Executes a command or pipeline
 *
 * Processes heredocs, detects builtins, and executes commands accordingly
 *
 * @param ctx Shell context
 * @param cmd Command to execute
 */
void	execute_commands(t_ctx *ctx, t_command *cmd)
{
	if (!cmd || !cmd->args || !cmd->args[0])
		return ;
	if (special_cases(ctx, cmd))
		return ;
	// First handle all heredocs
	if (read_all_heredocs(ctx) != 0)
	{
		cleanup_heredoc_resources(ctx);
		return ;
	}
	// Check if it's a simple builtin without pipeline
	if (cmd && !cmd->next && is_builtin_command(cmd->args[0]))
	{
		ctx->exit_status = execute_builtin(ctx, cmd);
		return ;
	}
	// Execute pipeline
	execute_pipeline(ctx, cmd);
	cleanup_heredoc_resources(ctx);
}
