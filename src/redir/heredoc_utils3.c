/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils3.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 12:55:14 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/23 12:57:43 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "minishell.h"

/**
 * @brief Processes a single line of heredoc content
 *
 * @param pipe_fd File descriptor to write to
 * @param line Input line content
 * @param ctx Shell context
 * @return 0 on success, -1 on error
 */
static int	process_heredoc_line(int pipe_fd, char *line, t_ctx *ctx)
{
	char	*expanded_line;

	expanded_line = expand_variables_in_line(ctx, line);
	if (!expanded_line)
		return (-1);

	write(pipe_fd, expanded_line, ft_strlen(expanded_line));
	write(pipe_fd, "\n", 1);
	free(expanded_line);

	return (0);
}

/**
 * @brief Processes heredoc content and writes to pipe
 *
 * @param pipe_fd Write end of pipe
 * @param delimiter String marking end of heredoc
 * @param ctx Shell context
 * @return 0 on success, non-zero on error
 */
int	process_heredoc_content(int pipe_fd, char *delimiter, t_ctx *ctx)
{
	char	*line;
	int		status;
	int		result;

	while (1)
	{
		status = read_heredoc_line(delimiter, &line);
		if (status != 0)
		{
			if (status < 0)
				return (1);
			return (0);
		}
		if (g_signal_status == 130)
		{
			free(line);
			return (-1);
		}

		result = process_heredoc_line(pipe_fd, line, ctx);
		free(line);

		if (result != 0)
			return (result);
	}
	return (0);
}

/**
 * @brief Checks if any heredoc is pending in the command
 *
 * @param cmd Command to check
 * @return t_bool True if there's a heredoc, false otherwise
 */
t_bool	has_pending_heredoc(t_command *cmd)
{
	t_redirection	*redir;

	if (!cmd || !cmd->redirection)
		return (false);

	redir = cmd->redirection;
	while (redir)
	{
		if (redir->type == TOK_HERE_DOC_FROM && redir->fd == -1)
			return (true);
		redir = redir->next;
	}

	return (false);
}
