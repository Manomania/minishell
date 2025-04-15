/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_redirect.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 09:52:58 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/15 13:57:55 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "debug.h"
#include "error.h"
#include "minishell.h"

/**
 * Opens a file for redirection
 *
 * @param redir Redirection information
 * @return int File descriptor or -1 on error
 */
int	open_redirection_file(t_redirection *redir)
{
	int	fd;

	fd = -1;
	if (redir->type == TOK_REDIR_FROM)
		fd = open(redir->filename, O_RDONLY);
	else if (redir->type == TOK_REDIR_TO)
		fd = open(redir->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (redir->type == TOK_HERE_DOC_TO)
		fd = open(redir->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
	else if (redir->type == TOK_HERE_DOC_FROM && redir->fd >= 0)
		return (redir->fd);
	if (fd == -1)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		perror(redir->filename);
	}
	return (fd);
}

/**
 * Sets up redirections for a built-in command
 *
 * @param cmd Command with redirections
 * @param saved_fds Array to store original file descriptors
 * @return int 0 on success, -1 on error
 */
int	builtin_setup_redirections(t_command *cmd, int saved_fds[2])
{
	t_redirection	*redir;
	int				result;

	(void)saved_fds;
	redir = cmd->redirection;
	while (redir)
	{
		if (is_here_doc_input(redir))
		{
			redir = redir->next;
			continue ;
		}
		result = apply_redirection(redir);
		if (result == -1)
			return (-1);
		redir = redir->next;
	}
	return (0);
}

/**
 * Restores original file descriptors after built-in execution
 *
 * @param saved_fds Array containing saved file descriptors
 * @return void
 */
void	builtin_restore_redirections(int saved_fds[2])
{
	if (saved_fds[0] != -1)
	{
		dup2(saved_fds[0], STDIN_FILENO);
		close(saved_fds[0]);
	}
	if (saved_fds[1] != -1)
	{
		dup2(saved_fds[1], STDOUT_FILENO);
		close(saved_fds[1]);
	}
}
