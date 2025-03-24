/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_redirections.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 17:36:47 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/24 15:22:41 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Sets up input/output redirections for a command
 *
 * @param redirections List of redirections
 * @return int 0 on success, -1 on failure
 */
int	handle_redirections(t_redirection *redirections)
{
	t_redirection	*redir;
	int				fd;

	redir = redirections;
	while (redir)
	{
		if (redir->type == TOK_REDIR_FROM)
		{
			fd = open(redir->filename, O_RDONLY);
			if (fd == -1)
				return (perror(redir->filename), -1);
			dup2(fd, STDIN_FILENO);
			close(fd);
		}
		else if (redir->type == TOK_REDIR_TO)
		{
			fd = open(redir->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fd == -1)
				return (perror(redir->filename), -1);
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		else if (redir->type == TOK_HERE_DOC_TO)
		{
			fd = open(redir->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
			if (fd == -1)
				return (perror(redir->filename), -1);
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		else if (redir->type == TOK_HERE_DOC_FROM)
		{
			ft_printf("TEST");
			// TODO: Implement heredoc with a pipe rather than a temp file
		}
		redir = redir->next;
	}
	return (0);
}
