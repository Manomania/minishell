/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ctx_clear.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 14:33:03 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/08 14:34:04 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Clears the context
 *
 * @param ctx context
 */
void	ctx_clear(t_ctx *ctx)
{
	free_all_token(ctx->tokens);
	command_free(ctx->cmd);
	clear_history();
	rl_free_line_state();
	rl_cleanup_after_signal();
	free(ctx);
}
