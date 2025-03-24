/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_redirections.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 17:36:47 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/24 15:47:19 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Handles input redirection
 *
 * @param filename File to redirect from
 * @return int 0 on success, -1 on failure
 */
static int	handle_input_redirection(char *filename)
{
	int	fd;

	fd = open(filename, O_RDONLY);
	if (fd == -1)
		return (perror(filename), -1);
	dup2(fd, STDIN_FILENO);
	close(fd);
	return (0);
}

/**
 * @brief Handles output redirection
 *
 * @param filename File to redirect to
 * @param append Whether to append or truncate
 * @return int 0 on success, -1 on failure
 */
static int	handle_output_redirection(char *filename, t_bool append)
{
	int	fd;
	int	flags;

	flags = O_WRONLY | O_CREAT;
	if (append)
		flags |= O_APPEND;
	else
		flags |= O_TRUNC;
	fd = open(filename, flags, 0644);
	if (fd == -1)
		return (perror(filename), -1);
	dup2(fd, STDOUT_FILENO);
	close(fd);
	return (0);
}

/**
 * @brief Sets up input/output redirections for a command
 *
 * @param redirections List of redirections
 * @return int 0 on success, -1 on failure
 */
int	handle_redirections(t_redirection *redirections)
{
	t_redirection	*redir;
	int				result;

	redir = redirections;
	while (redir)
	{
		if (redir->type == TOK_REDIR_FROM)
			result = handle_input_redirection(redir->filename);
		else if (redir->type == TOK_REDIR_TO)
			result = handle_output_redirection(redir->filename, false);
		else if (redir->type == TOK_HERE_DOC_TO)
			result = handle_output_redirection(redir->filename, true);
		else
			result = 0;
		if (result != 0)
			return (result);
		redir = redir->next;
	}
	return (0);
}
