/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 16:44:50 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/02 16:55:53 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "execute.h"

/**
 * @brief Checks if delimiter is quoted (affects variable expansion)
 *
 * @param delimiter Heredoc delimiter
 * @return t_bool true if quoted, false otherwise
 */
static t_bool	is_delimiter_quoted(char *delimiter)
{
	int	len;

	len = ft_strlen(delimiter);
	if (len >= 2 && ((delimiter[0] == '\'' && delimiter[len - 1] == '\'')
			|| (delimiter[0] == '"' && delimiter[len - 1] == '"')))
		return (true);
	return (false);
}

/**
 * @brief Setup signal handlers for heredoc input
 *
 * @param ctx Shell context
 */
static void	setup_heredoc_signals(t_ctx *ctx)
{
	(void)ctx;
	// TODO: Implement special signal handling for heredoc
	// This could include handling SIGINT differently
}

/**
 * @brief Creates and processes a heredoc
 *
 * @param ctx Shell context
 * @param redir Redirection structure
 * @return t_bool true on success, false on error
 */
static t_bool	handle_heredoc(t_ctx *ctx, t_redirection *redir)
{
	int		pipe_fd[2];
	char	*line;
	char	*processed_line;
	t_bool	quoted;
	char	*delimiter;

	if (pipe(pipe_fd) == -1)
		return (false);
	quoted = is_delimiter_quoted(redir->filename);
	if (quoted)
		delimiter = ft_substr(redir->filename, 1, ft_strlen(redir->filename)
				- 2);
	else
		delimiter = ft_strdup(redir->filename);
	if (!delimiter)
	{
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		return (false);
	}
	setup_heredoc_signals(ctx);
	while (1)
	{
		line = readline("> ");
		if (!line)
		{
			free(delimiter);
			close(pipe_fd[0]);
			close(pipe_fd[1]);
			return (false);
		}
		if (ft_strncmp(line, delimiter, ft_strlen(delimiter) + 1) == 0)
		{
			free(line);
			break ;
		}
		if (!quoted)
			processed_line = handle_quotes_and_vars(ctx, line);
		else
			processed_line = ft_strdup(line);
		free(line);
		if (!processed_line)
		{
			free(delimiter);
			close(pipe_fd[0]);
			close(pipe_fd[1]);
			return (false);
		}
		write(pipe_fd[1], processed_line, ft_strlen(processed_line));
		write(pipe_fd[1], "\n", 1);
		free(processed_line);
	}
	free(delimiter);
	close(pipe_fd[1]);
	redir->fd = pipe_fd[0];
	return (true);
}

/**
 * @brief Processes all heredocs in a command
 *
 * @param ctx Shell context
 * @param redir Redirection structure
 * @return t_bool true on success, false on error
 */
static t_bool	process_command_heredocs(t_ctx *ctx, t_redirection *redir)
{
	t_redirection	*current;

	current = redir;
	while (current)
	{
		if (current->type == TOK_HERE_DOC_FROM)
		{
			if (!handle_heredoc(ctx, current))
				return (false);
		}
		current = current->next;
	}
	return (true);
}

/**
 * @brief Processes all heredocs in a command list
 *
 * Collects user input for all heredocs before execution
 *
 * @param ctx Shell context
 * @param cmd Command list
 * @return t_bool true on success, false on error
 */
t_bool	process_heredocs(t_ctx *ctx, t_command *cmd)
{
	t_command	*current;

	current = cmd;
	while (current)
	{
		if (current->redirection)
		{
			if (!process_command_heredocs(ctx, current->redirection))
				return (false);
		}
		current = current->next;
	}
	return (true);
}
