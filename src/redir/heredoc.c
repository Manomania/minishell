/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 15:30:10 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/24 15:13:33 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "debug.h"
#include "error.h"
#include "minishell.h"

/**
 * @brief Processes a line read for heredoc
 *
 * @param line Line read from input
 * @param ctx Context containing environment information
 * @param pipe_fd File descriptor to write to
 * @return 0 on success, -1 on error
 */
static int	process_heredoc_line(char *line, t_ctx *ctx, int pipe_fd)
{
	char	*expanded_line;

	if (!line)
		return (-1);
	expanded_line = expand_variables_in_line(ctx, line);
	free(line);
	if (!expanded_line)
		return (-1);
	write(pipe_fd, expanded_line, ft_strlen(expanded_line));
	write(pipe_fd, "\n", 1);
	free(expanded_line);
	return (0);
}

/**
 * @brief Reads content for heredoc until delimiter is found
 *
 * @param pipe_fd File descriptor to write heredoc content to
 * @param delimiter String marking end of heredoc
 * @param ctx Context containing environment information
 * @return 0 on success, -1 on error, 1 on EOF
 */
static int	read_heredoc_content(int pipe_fd, char *delimiter, t_ctx *ctx)
{
	char	*line;
	int		delimiter_len;

	delimiter_len = ft_strlen(delimiter);
	while (1)
	{
		line = readline("> ");
		if (!line)
		{
			error_print(WARNING, "heredoc",
				"here-document delimited by end-of-file");
			return (1);
		}
		if (ft_strncmp(line, delimiter, delimiter_len + 1) == 0)
		{
			free(line);
			break ;
		}
		if (process_heredoc_line(line, ctx, pipe_fd) == -1)
			return (-1);
	}
	return (0);
}

/**
 * @brief Creates a here-document for input redirection
 *
 * @param ctx Context containing environment information
 * @param delimiter String marking end of heredoc
 * @return File descriptor to read from, or -1 on error
 */
static int	create_heredoc(t_ctx *ctx, char *delimiter)
{
	int	pipe_fds[2];
	int	result;

	if (pipe(pipe_fds) == -1)
	{
		error_print(ERROR, "heredoc", "pipe error");
		return (-1);
	}
	result = read_heredoc_content(pipe_fds[1], delimiter, ctx);
	close(pipe_fds[1]);
	if (result == -1)
	{
		close(pipe_fds[0]);
		return (-1);
	}
	return (pipe_fds[0]);
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
	int				heredoc_fd;
	int				dup_result;

	redir = cmd->redirection;
	while (redir)
	{
		if (redir->type == TOK_HERE_DOC_FROM)
		{
			heredoc_fd = create_heredoc(ctx, redir->filename);
			if (heredoc_fd == -1)
				return (-1);
			dup_result = dup2(heredoc_fd, STDIN_FILENO);
			close(heredoc_fd);
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
