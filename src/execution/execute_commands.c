/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_commands.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 16:44:19 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/02 16:55:53 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "error.h"
#include "execute.h"

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
	// First handle all heredocs
	if (!process_heredocs(ctx, cmd))
		return ;
	// Check if it's a simple builtin without pipeline
	if (cmd && !cmd->next && is_builtin_command(cmd->args[0]))
	{
		ctx->exit_status = execute_builtin(ctx, cmd);
		return ;
	}
	// Execute pipeline
	execute_pipeline(ctx, cmd);
}
