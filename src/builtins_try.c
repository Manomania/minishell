/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_try.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 12:19:14 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/12 17:18:49 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Tries the token against all builtins commands
 *
 * @param ctx Context
 * @param tok Token
 * @return t_bool Whether a command was successfully executed or not
 */
t_bool	builtins_try(t_ctx *ctx, t_command *cmd)
{
	if (ft_strncmp(cmd->args[0], "exit", __INT_MAX__) == 0)
		ctx_exit(ctx);
	return (false);
}
