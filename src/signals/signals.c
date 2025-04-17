/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 17:58:55 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/17 17:07:51 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* Global variable for signal status - required for POSIX signal handlers */
static int	g_signal_status = 0;

/**
 * @brief Signal handler for interactive mode (shell prompt)
 * Handles SIGINT (CTRL+C) by setting signal status
 *
 * @param sig Signal number
 */
static void	sig_interactive_handler(int sig)
{
	if (sig == SIGINT)
	{
		g_signal_status = 130;
		write(STDOUT_FILENO, "\n", 1);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

/**
 * @brief Sets up signal handlers for interactive mode
 * SIGINT (CTRL+C) is handled for status update
 * SIGQUIT (CTRL+\) is ignored
 */
void	setup_signals(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	g_signal_status = 0;
	sigemptyset(&sa_int.sa_mask);
	sigemptyset(&sa_quit.sa_mask);
	sa_int.sa_flags = 0;
	sa_quit.sa_flags = 0;
	sa_int.sa_handler = sig_interactive_handler;
	sa_quit.sa_handler = SIG_IGN;
	sigaction(SIGINT, &sa_int, NULL);
	sigaction(SIGQUIT, &sa_quit, NULL);
}

/**
 * @brief Resets signal handlers for child processes
 * SIGINT and SIGQUIT are set to their default behavior
 */
void	reset_signals(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	g_signal_status = 0;
	sigemptyset(&sa_int.sa_mask);
	sigemptyset(&sa_quit.sa_mask);
	sa_int.sa_flags = 0;
	sa_quit.sa_flags = 0;
	sa_int.sa_handler = SIG_DFL;
	sa_quit.sa_handler = SIG_DFL;
	sigaction(SIGINT, &sa_int, NULL);
	sigaction(SIGQUIT, &sa_quit, NULL);
}

/**
 * @brief Special signal handler setup for parent process execution
 * Temporarily disables interactive handlers for parent process
 */
void	setup_parent_signals(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	g_signal_status = 0;
	sigemptyset(&sa_int.sa_mask);
	sigemptyset(&sa_quit.sa_mask);
	sa_int.sa_flags = 0;
	sa_quit.sa_flags = 0;
	sa_int.sa_handler = SIG_IGN;
	sa_quit.sa_handler = SIG_IGN;
	sigaction(SIGINT, &sa_int, NULL);
	sigaction(SIGQUIT, &sa_quit, NULL);
}

/**
 * @brief Checks and updates exit status based on signal status
 * Allows main loop to capture SIGINT without directly checking global
 *
 * @param ctx Current shell context to update
 */
void	update_signal_status(t_ctx *ctx)
{
	if (g_signal_status == 130)
	{
		ctx->exit_status = 130;
		g_signal_status = 0;
	}
}
