/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 15:45:10 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/17 17:23:08 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "debug.h"
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

	fd = -1;
	if (type == TOK_REDIR_FROM)
		fd = open(filename, O_RDONLY);
	else if (type == TOK_REDIR_TO)
		fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (type == TOK_HERE_DOC_TO)
		fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd == -1)
		perror(filename);
	return (fd);
}

/**
 * @brief Redirects standard file descriptors
 *
 * @param fd File descriptor to redirect to/from
 * @param type Type of redirection
 * @return 0 on success, -1 on error
 */
static int	redirect_std_fd(int fd, t_token_type type)
{
	int	target_fd;
	int	dup_result;

	if (fd < 0)
		return (-1);
	if (type == TOK_REDIR_FROM)
		target_fd = STDIN_FILENO;
	else
		target_fd = STDOUT_FILENO;
	dup_result = dup2(fd, target_fd);
	close(fd);
	if (dup_result == -1)
	{
		perror("dup2");
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
		return (0);
	fd = open_redirect_file(redir->type, redir->filename);
	if (fd == -1)
		return (-1);
	redirect_result = redirect_std_fd(fd, redir->type);
	return (redirect_result);
}

/**
 * @brief Sets up all redirections for a command
 *
 * @param ctx Context for error handling
 * @param redirections List of redirections
 * @return 0 on success, -1 on error
 */
int	setup_redirections(t_redirection *redirections)
{
	t_redirection	*redir;
	int				result;
	char			error_msg[256];

	redir = redirections;
	if (!redir)
	{
		debug_log(DEBUG_INFO, "redir", "No redirections to process");
		return (0);
	}
	debug_log(DEBUG_INFO, "redir", "Setting up redirections");
	while (redir)
	{
		debug_log(DEBUG_VERBOSE, "redir", "Processing redirection");
		result = process_redirection(redir);
		if (result != 0)
		{
			ft_strlcpy(error_msg, "Failed to set up redirection for: ", 256);
			ft_strlcat(error_msg, redir->filename, 256);
			error_print(ERROR, "redir", error_msg);
			return (result);
		}
		redir = redir->next;
	}
	debug_log(DEBUG_INFO, "redir", "All redirections processed successfully");
	return (0);
}
