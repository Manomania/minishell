/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals_update.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 13:27:13 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/06 14:27:55 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "globals.h"
#include "minishell.h" // required bc globals.h does not include minishell.h

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
