/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals_setup.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 19:05:48 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/05 19:10:15 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Signal handler for interactive mode
 *
 * @param sig Signal number received
 */
static void	sig_interactive_handler(int sig)
{
	if (sig == SIGINT)
	{
		g_signal_status = 130;
		write(STDOUT_FILENO, "^C\n", 3);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

/**
 * @brief Sets up signal handlers for interactive mode
 */
void	setup_signals(void)
{
	struct sigaction	sa;

	rl_catch_signals = 0;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = sig_interactive_handler;
	sigaction(SIGINT, &sa, NULL);
	sa.sa_handler = SIG_IGN;
	sigaction(SIGQUIT, &sa, NULL);
}
