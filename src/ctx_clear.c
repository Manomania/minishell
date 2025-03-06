/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ctx_clear.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 14:33:03 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/06 14:54:26 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ctx_clear(t_ctx *ctx)
{
	if (ctx->tokens)
		free_all_token(ctx->tokens);
	clear_history();
	rl_clear_history();
	rl_free_line_state();
	rl_cleanup_after_signal();
	free(ctx);
}
