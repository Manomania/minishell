/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 15:45:10 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/28 13:22:58 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"

/**
 * @brief Sets up input redirection
 *
 * @param redir Current redirection structure
 * @param last_input_fd Previous input file descriptor
 * @param last_output_fd Current output file descriptor
 * @return New input fd or -1 on error
 */
static int	setup_input_redirection(t_redirection *redir, int last_input_fd,
		int last_output_fd)
{
	int	fd;

	if (last_input_fd != -1)
		close(last_input_fd);
	fd = open(redir->filename, O_RDONLY);
	if (fd == -1)
	{
		handle_redirection_error(redir->filename, -1, last_output_fd);
		return (-1);
	}
	return (fd);
}

/**
 * @brief Sets up output redirection
 *
 * @param redir Current redirection structure
 * @param last_input_fd Current input file descriptor
 * @param last_output_fd Previous output file descriptor
 * @return New output fd or -1 on error
 */
static int	setup_output_redirection(t_redirection *redir, int last_input_fd,
		int last_output_fd)
{
	int	fd;
	int	flags;

	if (last_output_fd != -1)
		close(last_output_fd);
	flags = O_WRONLY | O_CREAT;
	if (redir->type == TOK_REDIR_TO)
		flags |= O_TRUNC;
	else
		flags |= O_APPEND;
	fd = open(redir->filename, flags, 0644);
	if (fd == -1)
	{
		handle_redirection_error(redir->filename, last_input_fd, -1);
		return (-1);
	}
	return (fd);
}

/**
 * @brief Applies redirections to standard IO file descriptors
 *
 * @param last_input_fd Input file descriptor to use
 * @param last_output_fd Output file descriptor to use
 * @return 0 on success, -1 on error
 */
static int	apply_redirections(int last_input_fd, int last_output_fd)
{
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
			close(last_output_fd);
			return (-1);
		}
		close(last_output_fd);
	}
	return (0);
}

/**
 * @brief Process each redirection and update file descriptors
 *
 * @param redirections List of redirections to process
 * @param last_input_fd Pointer to last input file descriptor
 * @param last_output_fd Pointer to last output file descriptor
 * @return 0 on success, -1 on error
 */
static int	process_redirections(t_redirection *redirections,
		int *last_input_fd, int *last_output_fd)
{
	t_redirection	*redir;
	int				fd;

	redir = redirections;
	while (redir)
	{
		if (redir->type == TOK_REDIR_FROM)
		{
			fd = setup_input_redirection(redir, *last_input_fd,
					*last_output_fd);
			if (fd == -1)
				return (-1);
			*last_input_fd = fd;
		}
		else if (redir->type == TOK_REDIR_TO || redir->type == TOK_HERE_DOC_TO)
		{
			fd = setup_output_redirection(redir, *last_input_fd,
					*last_output_fd);
			if (fd == -1)
				return (-1);
			*last_output_fd = fd;
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
	int	last_input_fd;
	int	last_output_fd;
	int	result;

	last_input_fd = -1;
	last_output_fd = -1;
	if (!redirections)
		return (0);
	result = process_redirections(redirections, &last_input_fd,
			&last_output_fd);
	if (result == -1)
		return (-1);
	return (apply_redirections(last_input_fd, last_output_fd));
}
