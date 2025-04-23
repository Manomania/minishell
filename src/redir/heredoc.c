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
		rl_replace_line("", 0);
		rl_done = 1;
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
 * @brief Restores normal signal handlers after heredoc processing
 */
static void	restore_normal_signals(void)
{
	setup_signals();
}

/**
 * @brief Creates a heredoc pipe without using fork
 *
 * @param delimiter String marking end of heredoc
 * @return Pipe file descriptors array, or NULL on error
 */
static int	*create_heredoc_pipe(void)
{
	int	*pipe_fds;

	pipe_fds = malloc(sizeof(int) * 2);
	if (!pipe_fds)
		return (NULL);
	if (pipe(pipe_fds) == -1)
	{
		free(pipe_fds);
		return (NULL);
	}
	return (pipe_fds);
}

/**
 * @brief Handles reading heredoc input and writing to pipe
 *
 * @param pipe_fds Pipe file descriptors
 * @param delimiter String marking end of heredoc
 * @param ctx Shell context
 * @return 0 on success, -1 on error or interrupt
 */
static int	read_heredoc_content(int *pipe_fds, char *delimiter, t_ctx *ctx)
{
	char	*line;
	char	*expanded_line;
	int		status;

	while (1)
	{
		status = read_heredoc_line(delimiter, &line);
		if (status != 0)
		{
			if (status < 0)
				return (-1);
			return (0);
		}
		if (g_signal_status == 130)
		{
			free(line);
			return (-1);
		}
		expanded_line = expand_variables_in_line(ctx, line);
		free(line);
		if (!expanded_line)
			return (-1);
		write(pipe_fds[1], expanded_line, ft_strlen(expanded_line));
		write(pipe_fds[1], "\n", 1);
		free(expanded_line);
	}
	return (0);
}

/**
 * @brief Creates a heredoc without using fork
 *
 * @param ctx Shell context
 * @param delimiter String marking end of heredoc
 * @return File descriptor to read from, or -1 on error
 */
int	create_heredoc(t_ctx *ctx, char *delimiter)
{
	int		*pipe_fds;
	int		read_fd;
	int		result;

	pipe_fds = create_heredoc_pipe();
	if (!pipe_fds)
		return (error(NULL, "heredoc", ERR_PIPE));
	setup_heredoc_signals();
	result = read_heredoc_content(pipe_fds, delimiter, ctx);
	close(pipe_fds[1]);
	restore_normal_signals();
	if (result == -1 || g_signal_status == 130)
	{
		close(pipe_fds[0]);
		free(pipe_fds);
		ctx->exit_status = 130;
		g_signal_status = 0;
		return (-1);
	}
	read_fd = pipe_fds[0];
	free(pipe_fds);
	return (read_fd);
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
