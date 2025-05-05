/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_apply_output.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 16:24:57 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/05 16:26:37 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"

/**
 * @brief Open file with proper flags based on redirection type
 *
 * @param filename File name to open
 * @param redir_type Redirection type token
 * @return int File descriptor on success, -1 on error
 */
static int	open_output_file(char *filename, t_token_type redir_type)
{
	int	flags;
	int	fd;

	if (redir_type == TOK_REDIR_TO)
		flags = O_WRONLY | O_CREAT | O_TRUNC;
	else
		flags = O_WRONLY | O_CREAT | O_APPEND;
	fd = open(filename, flags, 0644);
	if (fd == -1)
		error(filename, "redirection", ERR_NO_PERMS);
	return (fd);
}

/**
 * @brief Process single output redirection
 *
 * @param redir Redirection structure
 * @return t_bool true on success, false on error
 */
static t_bool	process_output_redir(t_redirection *redir)
{
	int	fd;

	fd = open_output_file(redir->filename, redir->type);
	if (fd == -1)
		return (false);
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		close(fd);
		error("dup2", "redirection", ERR_FD);
		return (false);
	}
	close(fd);
	return (true);
}

/**
 * @brief Applies output redirection for a command
 *
 * @param cmd Command structure
 * @return t_bool true on success, false on error
 */
t_bool	apply_output_redirection(t_command *cmd)
{
	t_redirection	*redir;

	redir = cmd->redirection;
	while (redir)
	{
		if (redir->type == TOK_REDIR_TO || redir->type == TOK_HERE_DOC_TO)
		{
			if (!process_output_redir(redir))
				return (false);
		}
		redir = redir->next;
	}
	return (true);
}
