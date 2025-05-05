/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_apply_input.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 16:24:04 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/05 16:26:22 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"

/**
 * @brief Open file descriptor for input redirection
 *
 * Opens file or uses heredoc fd based on redirection type
 *
 * @param redir Redirection structure
 * @param is_heredoc Boolean flag for heredoc type
 * @return int File descriptor on success, -1 on error
 */
static int	get_input_fd(t_redirection *redir, t_bool is_heredoc)
{
	int	fd;

	if (is_heredoc)
		fd = redir->fd;
	else
		fd = open(redir->filename, O_RDONLY);
	if (!is_heredoc && fd == -1)
		error(redir->filename, "redirection", ERR_NO_FILE);
	return (fd);
}

/**
 * @brief Process single input redirection
 *
 * Handles dup2 operation for input redirection
 *
 * @param fd File descriptor to redirect
 * @return t_bool true on success, false on error
 */
static t_bool	process_input_fd(int fd)
{
	if (dup2(fd, STDIN_FILENO) == -1)
	{
		close(fd);
		error("dup2", "redirection", ERR_FD);
		return (false);
	}
	close(fd);
	return (true);
}

/**
 * @brief Handle input redirection cleanup
 *
 * Closes heredoc fd after use
 *
 * @param redir Redirection structure
 * @param is_heredoc Boolean flag for heredoc type
 */
static void	cleanup_input_redir(t_redirection *redir, t_bool is_heredoc)
{
	if (is_heredoc)
		redir->fd = -1;
}

/**
 * @brief Applies input redirection for a command
 *
 * @param cmd Command structure
 * @return t_bool true on success, false on error
 */
t_bool	apply_input_redirection(t_command *cmd)
{
	t_redirection	*redir;
	int				fd;
	t_bool			is_heredoc;

	redir = cmd->redirection;
	while (redir)
	{
		if (redir->type == TOK_REDIR_FROM || redir->type == TOK_HERE_DOC_FROM)
		{
			is_heredoc = (redir->type == TOK_HERE_DOC_FROM);
			fd = get_input_fd(redir, is_heredoc);
			if (!is_heredoc && fd == -1)
				return (false);
			if (!process_input_fd(fd))
				return (false);
			cleanup_input_redir(redir, is_heredoc);
		}
		redir = redir->next;
	}
	return (true);
}
