/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ctx_exit.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 17:15:04 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/18 12:30:49 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Clears the context and exits the app gracefully
 *
 * @param ctx context
 */
void	ctx_exit(t_ctx *ctx)
{
	int exit_code;

	exit_code = ctx->exit_status;
	ctx_clear(ctx);
	exit(exit_code);
}
