/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 15:45:10 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/16 15:35:04 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"

/**
 * @brief Opens a file based on redirection type
 *
 * @param type Redirection type (input, output, append)
 * @param filename Name of the file to open
 * @return File descriptor or -1 on error
 */
static int	open_redirect_file(t_token_type type, char *filename)
{
	int	fd;
	int	flags;

	fd = -1;
	if (type == TOK_REDIR_FROM)
		flags = O_RDONLY;
	else if (type == TOK_REDIR_TO)
		flags = O_WRONLY | O_CREAT | O_TRUNC;
	else if (type == TOK_HERE_DOC_TO)
		flags = O_WRONLY | O_CREAT | O_APPEND;
	else
		return (-1);
	fd = open(filename, flags, 0644);
	if (fd == -1)
	{
		if (errno == EACCES)
		{
			error(filename, "redir", ERR_NO_PERMS);
			return (-1);
		}
		else
		{
			error(filename, "redir", ERR_NO_FILE);
			return (-1);
		}
	}
	return (fd);
}

/**
 * @brief Redirects standard file descriptors
 *
 * @param fd File descriptor to redirect to/from
 * @param type Type of redirection
 * @param filename Filename (for error message if dup2 fails)
 * @return 0 on success, -1 on error
 */
static int	redirect_std_fd(int fd, t_token_type type, char *filename)
{
	int	target_fd;
	int	dup_result;

	if (fd < 0)
		return (-1);
	if (type == TOK_REDIR_FROM || type == TOK_HERE_DOC_FROM)
		target_fd = STDIN_FILENO;
	else
		target_fd = STDOUT_FILENO;
	dup_result = dup2(fd, target_fd);
	close(fd);
	if (dup_result == -1)
	{
		ft_printf_fd(STDERR_FILENO, "minishell: %s: ", filename);
		perror("dup2 failed");
		return (-1);
	}
	return (0);
}

/**
 * @brief Processes a single redirection
 *
 * @param redir Redirection to process
 * @return 0 on success, -1 on error
 */
static int	process_redirection(t_redirection *redir)
{
	int	fd;
	int	redirect_result;

	if (redir->type == TOK_HERE_DOC_FROM)
	{
		if (redir->fd >= 0)
		{
			redirect_result = redirect_std_fd(redir->fd, redir->type,
					"heredoc");
			redir->fd = -1;
			return (redirect_result);
		}
		return (0);
	}
	fd = open_redirect_file(redir->type, redir->filename);
	if (fd == -1)
		return (-1);
	redirect_result = redirect_std_fd(fd, redir->type, redir->filename);
	return (redirect_result);
}

/**
 * @brief Process each redirection in a linked list sequentially.
 * Stops and returns immediately on the first error.
 *
 * @param redirections Redirection list to process
 * @return 0 on success, -1 on failure (first error encountered)
 */
static int	process_redirection_list(t_redirection *redirections)
{
	t_redirection	*redir;
	int				result;

	redir = redirections;
	while (redir)
	{
		result = process_redirection(redir);
		if (result != 0)
		{
			return (-1);
		}
		redir = redir->next;
	}
	return (0);
}

/**
 * @brief Sets up all redirections for a command. Stops on first error.
 *
 * @param redirections List of redirections to process
 * @return 0 on success, -1 on error
 */
int	setup_redirections(t_redirection *redirections)
{
	if (!redirections)
		return (0);
	return (process_redirection_list(redirections));
}
