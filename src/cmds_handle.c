/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmds_handle.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 16:10:59 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/08 12:22:01 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Get the first token that is a word
 *
 * @param ctx Context
 * @return t_token* First word token, or NULL
 */
static t_token	*get_first_word(t_ctx *ctx)
{
	t_token	*tok;

	tok = ctx->tokens;
	while (tok)
	{
		if (tok->type == TOK_WORD)
			break ;
		tok = tok->next;
	}
	return (tok);
}

/**
 * @brief Handles a command
 *
 * @param ctx Context
 */
void	cmds_handle(t_ctx *ctx)
{
	t_token	*tok;

	if (!ctx || !ctx->tokens)
		ctx_error_exit(ctx, ERR_NONE);
	tok = get_first_word(ctx);
	if (builtins_try(ctx, tok))
		return ;
}
