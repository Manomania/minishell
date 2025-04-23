/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 15:30:10 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/23 12:57:15 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"

/**
 * @brief Signal handler for heredoc mode
 *
 * @param sig Signal number
 */
static void	sig_heredoc_handler(int sig)
{
	if (sig == SIGINT)
	{
		g_signal_status = 130;
		write(STDOUT_FILENO, "\n", 1);
		exit(130);
	}
}

/**
 * @brief Sets up signal handlers for heredoc mode
 */
static void	setup_heredoc_signals(void)
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
 * @brief Handles the child process for heredoc
 *
 * @param pipe_fds Pipe file descriptors
 * @param delimiter String marking end of heredoc
 * @param ctx Shell context
 * @return void, exits process
 */
void	handle_heredoc_child(int pipe_fds[2], char *delimiter, t_ctx *ctx)
{
	close(pipe_fds[0]);
	setup_heredoc_signals();
	process_heredoc_content(pipe_fds[1], delimiter, ctx);
	close(pipe_fds[1]);
	exit(EXIT_SUCCESS);
}

/**
 * @brief Creates a heredoc and handles signal interruption
 *
 * @param ctx Shell context
 * @param delimiter String marking end of heredoc
 * @return File descriptor to read from, or -1 on error
 */
int	create_heredoc(t_ctx *ctx, char *delimiter)
{
	int		pipe_fds[2];
	pid_t	pid;
	int		result;

	if (setup_heredoc_pipes(pipe_fds) < 0)
		return (-1);
	pid = fork();
	if (pid == -1)
	{
		close(pipe_fds[0]);
		close(pipe_fds[1]);
		return (error(NULL, "heredoc", ERR_CHILD));
	}
	if (pid == 0)
		handle_heredoc_child(pipe_fds, delimiter, ctx);
	close(pipe_fds[1]);
	result = wait_heredoc_child(pipe_fds, ctx);
	return (result);
}

/**
 * @brief Handles here_doc redirections for a command
 *
 * @param ctx Context containing environment information
 * @param cmd Command containing redirections
 * @return 0 on success, non-zero on error
 */
int	setup_heredocs(t_ctx *ctx, t_command *cmd)
{
	t_redirection	*redir;
	int				dup_result;

	(void)ctx;
	redir = cmd->redirection;
	while (redir)
	{
		if (redir->type == TOK_HERE_DOC_FROM && redir->fd >= 0)
		{
			dup_result = dup2(redir->fd, STDIN_FILENO);
			close(redir->fd);
			redir->fd = -1;
			if (dup_result == -1)
			{
				perror("dup2");
				return (-1);
			}
		}
		redir = redir->next;
	}
	return (0);
}
