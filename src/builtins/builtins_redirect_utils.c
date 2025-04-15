/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_redirect_utils.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 12:35:35 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/15 14:18:11 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "minishell.h"

/**
 * Checks if redirection is a here-doc input
 *
 * @param redir Redirection to check
 * @return t_bool true if here-doc input, false otherwise
 */
t_bool	is_here_doc_input(t_redirection *redir)
{
	if (redir->type == TOK_HERE_DOC_FROM)
		return (1);
	return (0);
}

/**
 * Applies input redirection
 *
 * @param fd File descriptor to redirect from
 * @return 0 on success, -1 on error
 */
int	apply_input_redirection(int fd)
{
	if (dup2(fd, STDIN_FILENO) == -1)
	{
		close(fd);
		return (-1);
	}
	close(fd);
	return (0);
}

/**
 * Applies output redirection
 *
 * @param fd File descriptor to redirect to
 * @return 0 on success, -1 on error
 */
int	apply_output_redirection(int fd)
{
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		close(fd);
		return (-1);
	}
	close(fd);
	return (0);
}

/**
 * Reports redirection error
 *
 * @param filename Filename that caused the error
 * @return -1 to indicate error
 */
static int	report_redir_error(char *filename)
{
	ft_putstr_fd(RED "minishell: ", STDERR_FILENO);
	ft_putstr_fd(filename, STDERR_FILENO);
	ft_putstr_fd(": ", STDERR_FILENO);
	if (errno == ENOENT)
		ft_putstr_fd("No such file or directory" RESET, STDERR_FILENO);
	else if (errno == EACCES)
		ft_putstr_fd("Permission denied" RESET, STDERR_FILENO);
	else
		ft_putstr_fd(strerror(errno), STDERR_FILENO);
	ft_putstr_fd("\n", STDERR_FILENO);
	return (-1);
}

/**
 * Applies a redirection for a command
 *
 * This function first opens the file and then redirects the standard
 * file descriptor to it. It properly handles error cases.
 *
 * @param redir Redirection to apply
 * @return 0 on success, -1 on error
 */
int	apply_redirection(t_redirection *redir)
{
	int	fd;
	int	result;

	fd = open_redirection_file(redir);
	if (fd == -1)
		return (report_redir_error(redir->filename));
	if (redir->type == TOK_REDIR_FROM || redir->type == TOK_HERE_DOC_FROM)
		result = apply_input_redirection(fd);
	else
		result = apply_output_redirection(fd);
	if (result == -1)
		return (report_redir_error(redir->filename));
	return (0);
}
