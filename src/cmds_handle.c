/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmds_handle.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 16:10:59 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/08 11:18:39 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	cmds_handle(t_ctx *ctx)
{
	if (!ctx || !ctx->tokens)
		ctx_error_exit(ctx, ERR_NONE);
	if (ctx->tokens->type == TOK_WORD && ft_strncmp(ctx->tokens->value, "exit",
			__INT_MAX__) == 0)
		ctx_exit(ctx);
}
