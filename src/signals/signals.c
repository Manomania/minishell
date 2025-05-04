/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 17:58:55 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/04 18:42:02 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Signal handler for heredoc mode (SIGINT)
 * Handles SIGINT (CTRL+C) by closing pipes and setting signal status
 *
 * @param sig Signal number
 */
static void	sig_heredoc_handler(int sig)
{
	if (sig == SIGINT)
	{
		g_signal_status = 130;
		exit(130);
	}
}

/**
 * @brief Sets up signal handlers for heredoc mode
 * SIGINT (CTRL+C) exits the process
 * SIGQUIT (CTRL+\) is ignored
 */
void	setup_heredoc_signals(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	sigemptyset(&sa_int.sa_mask);
	sigemptyset(&sa_quit.sa_mask);
	sa_int.sa_flags = 0;
	sa_quit.sa_flags = 0;
	sa_int.sa_handler = sig_heredoc_handler;
	sa_quit.sa_handler = SIG_IGN;
	sigaction(SIGINT, &sa_int, NULL);
	sigaction(SIGQUIT, &sa_quit, NULL);
}

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
		write(STDOUT_FILENO, "^C\n", 3);
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

	rl_catch_signals = 0;
	rl_catch_sigwinch = 1;
	sigemptyset(&sa_int.sa_mask);
	sigemptyset(&sa_quit.sa_mask);
	sa_int.sa_flags = SA_RESTART;
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
 * @brief Signal handler for parent process during command execution
 * Prints ^C or ^\ when received by foreground process
 *
 * @param sig Signal number
 */
static void	sig_parent_handler(int sig)
{
	if (sig == SIGINT)
	{
		write(STDOUT_FILENO, "\n", 1);
		g_signal_status = 130;
	}
	else if (sig == SIGQUIT)
	{
		write(STDOUT_FILENO, "Quit (core dumped)\n", 19);
		g_signal_status = 131;
	}
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
	sa_int.sa_handler = sig_parent_handler;
	sa_quit.sa_handler = sig_parent_handler;
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
	else if (g_signal_status == 131)
	{
		ctx->exit_status = 131;
		g_signal_status = 0;
	}
}
