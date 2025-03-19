/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_try.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 12:19:14 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/19 18:34:16 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "debug.h"
#include "error.h"
#include "minishell.h"

/**
 * @brief Tries to execute a builtin command
 *
 * @param ctx Context for shell environment
 * @param cmd Command to check for builtin
 * @return t_bool true if builtin was executed, false otherwise
 */
t_bool	builtins_try(t_ctx *ctx, t_command *cmd)
{
	char	cmd_name[64];
	int		exit_code;

	if (!ctx || !cmd || !cmd->args || !cmd->args[0])
	{
		error_print(ERROR, "builtin", "Invalid command");
		return (false);
	}
	ft_strlcpy(cmd_name, "Checking builtin: ", sizeof(cmd_name));
	ft_strlcat(cmd_name, cmd->args[0], sizeof(cmd_name));
	debug_log(DEBUG_INFO, "builtin", cmd_name);
	if (ft_strncmp(cmd->args[0], "exit", __INT_MAX__) == 0)
	{
		debug_log(DEBUG_INFO, "builtin", "Executing exit builtin");
		exit_code = 0;
		if (cmd->arg_count >= 1 && cmd->args[1])
			exit_code = ft_atoi(cmd->args[1]);
		ctx->exit_status = exit_code;
		ctx->exit_requested = true;
		return (true);
	}
	debug_log(DEBUG_INFO, "builtin", "No matching builtin found");
	return (false);
}
