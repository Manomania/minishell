/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 15:30:10 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/23 10:02:33 by maximart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"

/**
 * @brief Displays warning message for EOF in heredoc
 */
static void	display_heredoc_eof_warning(void)
{
	char	*warning_part1;
	char	*warning_part2;
	char	*warning_part3;

	warning_part1 = "minishell: warning: heredoc: ";
	warning_part2 = "here-document delimited by end-of-file ";
	warning_part3 = "(`eof')\n";
	ft_printf_fd(STDERR_FILENO, YELLOW "%s%s%s" RESET, warning_part1,
		warning_part2, warning_part3);
}

/**
 * @brief Signal handler for heredoc mode
 *
 * @param sig Signal number
 */
static void sig_heredoc_handler(int sig)
{
	if (sig == SIGINT)
	{
		g_signal_status = 130;  // Signal status for SIGINT
		write(STDOUT_FILENO, "\n", 1);
		exit(130);  // Exit the child process with signal code
	}
}

/**
 * @brief Sets up signal handlers for heredoc mode
 */
static void setup_heredoc_signals(void)
{
	struct sigaction sa_int;
	struct sigaction sa_quit;

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
 * @brief Processes heredoc content and writes to pipe
 *
 * @param pipe_fd Write end of pipe
 * @param delimiter String marking end of heredoc
 * @param ctx Shell context
 * @return 0 on success, non-zero on error
 */
static int process_heredoc_content(int pipe_fd, char *delimiter, t_ctx *ctx)
{
	char *line;
	int delimiter_len;

	delimiter_len = ft_strlen(delimiter);
	while (1)
	{
		line = readline("> ");
		if (!line)
		{
			display_heredoc_eof_warning();
			return (1);
		}
		if (ft_strncmp(line, delimiter, delimiter_len + 1) == 0)
		{
			free(line);
			break;
		}
		char *expanded_line = expand_variables_in_line(ctx, line);
		free(line);
		if (!expanded_line)
			return (-1);
		write(pipe_fd, expanded_line, ft_strlen(expanded_line));
		write(pipe_fd, "\n", 1);
		free(expanded_line);
	}
	return (0);
}

/**
 * @brief Creates a heredoc and handles signal interruption
 *
 * @param ctx Shell context
 * @param delimiter String marking end of heredoc
 * @return File descriptor to read from, or -1 on error
 */
int create_heredoc(t_ctx *ctx, char *delimiter)
{
	int pipe_fds[2];
	pid_t pid;
	int status;
	int heredoc_fd;

	if (pipe(pipe_fds) == -1)
		return (error(NULL, "heredoc", ERR_PIPE));
	setup_parent_signals();  // Parent uses non-interactive mode
	pid = fork();
	if (pid == -1)
	{
		close(pipe_fds[0]);
		close(pipe_fds[1]);
		return (error(NULL, "heredoc", ERR_CHILD));
	}
	if (pid == 0)
	{
		close(pipe_fds[0]);
		setup_heredoc_signals();
		process_heredoc_content(pipe_fds[1], delimiter, ctx);
		close(pipe_fds[1]);
		exit(EXIT_SUCCESS);
	}
	close(pipe_fds[1]);
	waitpid(pid, &status, 0);
	if (WIFSIGNALED(status) || (WIFEXITED(status) && WEXITSTATUS(status) == 130))
	{
		close(pipe_fds[0]);
		ctx->exit_status = 130;
		g_signal_status = 130;
		return (-1);
	}
	heredoc_fd = pipe_fds[0];
	setup_signals();  // Restore normal signals
	return (heredoc_fd);
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
