/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 15:34:26 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/05 15:39:23 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Updates context exit status based on signal status
 *
 * @param ctx Context to update
 */
void	update_signal_status(t_ctx *ctx)
{
	if (g_signal_status != 0)
	{
		ctx->exit_status = g_signal_status;
		g_signal_status = 0;
	}
}
