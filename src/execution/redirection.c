/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 16:45:10 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/05 19:02:12 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "execute.h"
#include <fcntl.h>

/**
 * @brief Applies input redirection for a command
 *
 * @param cmd Command structure
 * @return t_bool true on success, false on error
 */
static t_bool	apply_input_redirection(t_command *cmd)
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
			if (is_heredoc)
				fd = redir->fd;
			else
				fd = open(redir->filename, O_RDONLY);
			if (!is_heredoc && fd == -1)
				return (error(redir->filename, "redirection", ERR_NO_FILE),
					false);
			if (dup2(fd, STDIN_FILENO) == -1)
			{
				close(fd);
				return (error("dup2", "redirection", ERR_FD), false);
			}
			close(fd);
			if (is_heredoc)
				redir->fd = -1;
		}
		redir = redir->next;
	}
	return (true);
}

/**
 * @brief Applies output redirection for a command
 *
 * @param cmd Command structure
 * @return t_bool true on success, false on error
 */
static t_bool	apply_output_redirection(t_command *cmd)
{
	t_redirection	*redir;
	int				fd;
	int				flags;

	redir = cmd->redirection;
	while (redir)
	{
		if (redir->type == TOK_REDIR_TO || redir->type == TOK_HERE_DOC_TO)
		{
			if (redir->type == TOK_REDIR_TO)
				flags = O_WRONLY | O_CREAT | O_TRUNC;
			else
				flags = O_WRONLY | O_CREAT | O_APPEND;
			fd = open(redir->filename, flags, 0644);
			if (fd == -1)
				return (error(redir->filename, "redirection", ERR_NO_PERMS),
					false);
			if (dup2(fd, STDOUT_FILENO) == -1)
			{
				close(fd);
				return (error("dup2", "redirection", ERR_FD), false);
			}
			close(fd);
		}
		redir = redir->next;
	}
	return (true);
}

/**
 * @brief Applies all redirections for a command
 *
 * @param cmd Command structure
 * @return t_bool true on success, false on error
 */
t_bool	apply_redirections(t_command *cmd)
{
	if (!cmd || !cmd->redirection)
		return (true);
	if (!apply_input_redirection(cmd))
		return (false);
	if (!apply_output_redirection(cmd))
		return (false);
	return (true);
}

/**
 * @brief Saves original standard file descriptors
 *
 * @param stdin_fd Pointer to store original stdin fd
 * @param stdout_fd Pointer to store original stdout fd
 * @return t_bool true on success, false on error
 */
t_bool	save_original_fds(int *stdin_fd, int *stdout_fd)
{
	*stdin_fd = dup(STDIN_FILENO);
	if (*stdin_fd == -1)
		return (error("dup", "redirection", ERR_FD), false);
	*stdout_fd = dup(STDOUT_FILENO);
	if (*stdout_fd == -1)
	{
		close(*stdin_fd);
		return (error("dup", "redirection", ERR_FD), false);
	}
	return (true);
}

/**
 * @brief Restores original standard file descriptors
 *
 * @param stdin_fd Original stdin fd
 * @param stdout_fd Original stdout fd
 */
void	restore_original_fds(int stdin_fd, int stdout_fd)
{
	if (stdin_fd != -1)
	{
		dup2(stdin_fd, STDIN_FILENO);
		close(stdin_fd);
	}
	if (stdout_fd != -1)
	{
		dup2(stdout_fd, STDOUT_FILENO);
		close(stdout_fd);
	}
}
