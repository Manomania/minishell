/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_signals.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maximart <maximart@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 12:09:37 by maximart          #+#    #+#             */
/*   Updated: 2025/05/05 14:25:18 by maximart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	sig_heredoc_handler(int sig)
{
	if (sig == SIGINT)
	{
		g_signal_status = 130;
		write(STDOUT_FILENO, "^C", 2);
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

void	reset_heredoc_state(void)
{
	rl_event_hook = NULL;
	g_signal_status = 0;
	rl_cleanup_after_signal();
	rl_deprep_terminal();
	rl_reset_terminal(NULL);
	setup_signals();
}

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

t_bool	is_heredoc_interrupted(void)
{
	return (g_signal_status == 130);
}

int	interrupt_check_hook(void)
{
	if (is_heredoc_interrupted())
	{
		rl_done = 1;
		return (1);
	}
	return (0);
}
