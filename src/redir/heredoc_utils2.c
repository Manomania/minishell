/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 13:59:38 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/23 12:58:56 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "error.h"
#include "minishell.h"

/**
 * @brief Closes all open heredoc file descriptors
 *
 * @param cmd Command containing redirections
 */
void	close_heredoc_fds(t_command *cmd)
{
	t_redirection	*redir;

	while (cmd)
	{
		redir = cmd->redirection;
		while (redir)
		{
			if (redir->fd > 2)
			{
				close(redir->fd);
				redir->fd = -1;
			}
			redir = redir->next;
		}
		cmd = cmd->next;
	}
}

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
 * @brief Handles reading and checking heredoc content
 *
 * @param delimiter String marking end of heredoc
 * @param line Pointer to store the read line
 * @return 1 if delimiter is found, 0 to continue, -1 on error
 */
int	read_heredoc_line(char *delimiter, char **line)
{
	int	delimiter_len;

	delimiter_len = ft_strlen(delimiter);
	*line = readline("> ");
	if (!(*line))
	{
		display_heredoc_eof_warning();
		return (-1);
	}
	if (ft_strncmp(*line, delimiter, delimiter_len + 1) == 0)
	{
		free(*line);
		return (1);
	}
	return (0);
}

/**
 * @brief Sets up pipes and initializes the heredoc process
 *
 * @param pipe_fds Array to store pipe file descriptors
 * @return 0 on success, negative value on error
 */
int	setup_heredoc_pipes(int pipe_fds[2])
{
	if (pipe(pipe_fds) == -1)
		return (error(NULL, "heredoc", ERR_PIPE));
	setup_parent_signals();
	return (0);
}

/**
 * @brief Waits for heredoc child and processes result
 *
 * @param pipe_fds Pipe file descriptors
 * @param ctx Shell context
 * @return Read file descriptor or -1 on error
 */
int	wait_heredoc_child(int pipe_fds[2], t_ctx *ctx)
{
	int	status;
	int	heredoc_fd;

	waitpid(-1, &status, 0);
	if (WIFSIGNALED(status) || (WIFEXITED(status)
			&& WEXITSTATUS(status) == 130))
	{
		close(pipe_fds[0]);
		ctx->exit_status = 130;
		g_signal_status = 130;
		return (-1);
	}
	heredoc_fd = pipe_fds[0];
	setup_signals();
	return (heredoc_fd);
}
