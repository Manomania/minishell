/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 17:58:55 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/05 19:10:11 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Signal handler for parent process during command execution
 *
 * @param sig Signal number received
 */
static void	sig_parent_handler(int sig)
{
	if (sig == SIGINT)
	{
		ft_printf("\n");
		g_signal_status = 130;
	}
	else if (sig == SIGQUIT)
	{
		ft_printf("Quit (core dumped)\n");
		g_signal_status = 131;
	}
}

/**
 * @brief Sets up signal handlers for parent process during command execution
 */
void	setup_parent_signals(void)
{
	struct sigaction	sa;

	g_signal_status = 0;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = sig_parent_handler;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
}

/**
 * @brief Resets signal handlers to default behavior
 */
void	reset_signals(void)
{
	struct sigaction	sa;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = 0;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
}

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
