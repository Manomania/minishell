/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_try.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 12:19:14 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/15 15:57:59 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "debug.h"
#include "error.h"
#include "minishell.h"

/**
 * Checks if the command is a recognized builtin
 *
 * @param cmd_name Name of the command to check
 * @return t_bool true if command is a builtin, false otherwise
 */
static t_bool	is_recognized_builtin(char *cmd_name)
{
	if (is_builtin(cmd_name, (char *)"exit") == 0 || is_builtin(cmd_name,
			(char *)"echo") == 0 || is_builtin(cmd_name, (char *)"cd") == 0
		|| is_builtin(cmd_name, (char *)"pwd") == 0 || is_builtin(cmd_name,
			(char *)"export") == 0 || is_builtin(cmd_name, (char *)"unset") == 0
		|| is_builtin(cmd_name, (char *)"env") == 0)
		return (1);
	return (0);
}

/**
 * Validates the command arguments
 *
 * @param ctx Context for shell environment
 * @param cmd Command to validate
 * @return t_bool true if valid, false otherwise
 */
static t_bool	validate_command(t_ctx *ctx, t_command *cmd)
{
	if (!ctx || !cmd || !cmd->args || !cmd->args[0])
		return (0);
	return (1);
}

/**
 * Tries to execute a builtin command
 *
 * @param ctx Context for shell environment
 * @param cmd Command to check for builtin
 * @return t_bool true if builtin was executed, false otherwise
 */
t_bool	builtins_try(t_ctx *ctx, t_command *cmd)
{
	int	exit_status;

	if (!validate_command(ctx, cmd))
		return (0);
	if (is_recognized_builtin(cmd->args[0]))
	{
		if (execute_builtin(ctx, cmd, &exit_status))
		{
			ctx->exit_status = exit_status;
			return (1);
		}
	}
	return (0);
}
