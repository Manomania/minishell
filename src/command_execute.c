/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_execute.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 16:10:59 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/08 15:46:59 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// /**
//  * @brief Get the first token that is a word
//  *
//  * @param ctx Context
//  * @return t_token* First word token, or NULL
//  */
// static t_token	*get_first_word(t_ctx *ctx)
// {
// 	t_token	*tok;

// 	tok = ctx->tokens;
// 	while (tok)
// 	{
// 		if (tok->type == TOK_WORD)
// 			break ;
// 		tok = tok->next;
// 	}
// 	return (tok);
// }

/**
 * @brief Executes a command structure
 *
 * @param ctx Context
 * @param cmd Command to execute
 * @return int Exit status
 */
int	command_execute(t_ctx *ctx, t_command *cmd)
{
	if (!cmd->cmd)
		return (0);
	if (builtins_try(ctx, cmd))
		return (0);
	cmd->cmd = bin_find(ctx, cmd->cmd);
	if (!cmd->cmd)
		ctx_error(ERR_CMD_NOT_FOUND);
	return (0);
}
