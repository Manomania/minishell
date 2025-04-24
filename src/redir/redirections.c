/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 15:45:10 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/24 17:01:03 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"

/**
 * @brief Sets up all redirections for a command. Stops on first error.
 *
 * @param redirections List of redirections to process
 * @return 0 on success, -1 on error
 */
int	setup_redirections(t_redirection *redirections)
{
	t_redirection	*redir;
	int				last_input_fd;
	int				last_output_fd;
	int				fd;
	int				flags;

	redir = redirections;
	last_input_fd = -1;
	last_output_fd = -1;
	if (!redirections)
		return (0);
	while (redir)
	{
		if (redir->type == TOK_REDIR_FROM)
		{
			if (last_input_fd != -1)
				close(last_input_fd);
			fd = open(redir->filename, O_RDONLY);
			if (fd == -1)
			{
				if (errno == EACCES)
					error(redir->filename, "redir", ERR_NO_PERMS);
				else
					error(redir->filename, "redir", ERR_NO_FILE);
				if (last_output_fd != -1)
					close(last_output_fd);
				return (-1);
			}
			last_input_fd = fd;
		}
		else if (redir->type == TOK_REDIR_TO || redir->type == TOK_HERE_DOC_TO)
		{
			if (last_output_fd != -1)
				close(last_output_fd);
			flags = (redir->type == TOK_REDIR_TO) ? (O_WRONLY | O_CREAT | O_TRUNC) : (O_WRONLY | O_CREAT | O_APPEND);
			fd = open(redir->filename, flags, 0644);
			if (fd == -1)
			{
				if (errno == EACCES)
					error(redir->filename, "redir", ERR_NO_PERMS);
				else
					error(redir->filename, "redir", ERR_NO_FILE);
				if (last_input_fd != -1)
					close(last_input_fd);
				return (-1);
			}
			last_output_fd = fd;
		}
		redir = redir->next;
	}
	if (last_input_fd != -1)
	{
		if (dup2(last_input_fd, STDIN_FILENO) == -1)
		{
			error("dup2", "redir", ERR_NO_FILE);
			close(last_input_fd);
			if (last_output_fd != -1)
				close(last_output_fd);
			return (-1);
		}
		close(last_input_fd);
	}
	if (last_output_fd != -1)
	{
		if (dup2(last_output_fd, STDOUT_FILENO) == -1)
		{
			error("dup2", "redir", ERR_NO_FILE);
			if (last_input_fd != -1)
				close(last_input_fd);
			close(last_output_fd);
			return (-1);
		}
		close(last_output_fd);
	}
	return (0);
}
