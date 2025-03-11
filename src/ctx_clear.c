/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ctx_clear.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 14:33:03 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/11 12:11:21 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Clears the context and frees all allocated resources
 *
 * @param ctx context
 */
void	ctx_clear(t_ctx *ctx)
{
	if (!ctx)
		return ;
	if (ctx->tokens)
		free_all_token(ctx->tokens);
	ctx->tokens = NULL;
	if (ctx->cmd)
		command_free(ctx->cmd);
	ctx->cmd = NULL;
	clear_history();
	rl_free_line_state();
	rl_cleanup_after_signal();
	if (ctx->fd_file_in != -1)
		close(ctx->fd_file_in);
	if (ctx->fd_file_out != -1)
		close(ctx->fd_file_out);
	free(ctx);
}
