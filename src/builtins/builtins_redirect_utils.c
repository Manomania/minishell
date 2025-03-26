/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_redirect_utils.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 12:35:35 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/24 12:36:30 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "minishell.h"

/**
 * @brief Checks if redirection is a here-doc input
 *
 * @param redir Redirection to check
 * @return t_bool true if here-doc input, false otherwise
 */
t_bool	is_here_doc_input(t_redirection *redir)
{
	if (redir->type == TOK_HERE_DOC_FROM)
		return (true);
	return (false);
}

/**
 * @brief Applies input redirection
 *
 * @param fd File descriptor to redirect from
 * @return int 0 on success, -1 on error
 */
int	apply_input_redirection(int fd)
{
	if (dup2(fd, STDIN_FILENO) == -1)
	{
		close(fd);
		return (-1);
	}
	return (0);
}

/**
 * @brief Applies output redirection
 *
 * @param fd File descriptor to redirect to
 * @return int 0 on success, -1 on error
 */
int	apply_output_redirection(int fd)
{
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		close(fd);
		return (-1);
	}
	return (0);
}

/**
 * @brief Applies a single redirection
 *
 * @param redir Redirection to apply
 * @return int 0 on success, -1 on error
 */
int	apply_redirection(t_redirection *redir)
{
	int	fd;
	int	result;

	fd = open_redirection_file(redir);
	if (fd == -1)
		return (-1);
	if (redir->type == TOK_REDIR_FROM)
		result = apply_input_redirection(fd);
	else if (redir->type == TOK_REDIR_TO || redir->type == TOK_HERE_DOC_TO)
		result = apply_output_redirection(fd);
	else
		result = 0;
	close(fd);
	return (result);
}
