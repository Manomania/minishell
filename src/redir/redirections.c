/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 15:45:10 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/15 14:43:26 by elagouch         ###   ########.fr       */
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

	int flags; // Added flags variable for clarity
	fd = -1;
	// Use flags for open modes
	if (type == TOK_REDIR_FROM)
		flags = O_RDONLY;
	else if (type == TOK_REDIR_TO)
		flags = O_WRONLY | O_CREAT | O_TRUNC;
	else if (type == TOK_HERE_DOC_TO)
		flags = O_WRONLY | O_CREAT | O_APPEND;
	else // Should not happen for file opening, but good practice
		return (-1);
	fd = open(filename, flags, 0644); // Use flags here
	if (fd == -1)
	{
		// Print error only once here, using the error module if available
		error(filename, "redir",
			(errno == EACCES) ? ERR_NO_PERMS : ERR_NO_FILE);
		// Using perror is simpler for now to match bash output closely
		ft_putstr_fd(RED "minishell: ", STDERR_FILENO);
		perror(filename); // perror prints the filename and the system error
		ft_putstr_fd(RESET, STDERR_FILENO);
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

	if (fd < 0) // Should be checked before calling
		return (-1);
	if (type == TOK_REDIR_FROM || type == TOK_HERE_DOC_FROM)
		// Include HERE_DOC_FROM for consistency
		target_fd = STDIN_FILENO;
	else // TOK_REDIR_TO or TOK_HERE_DOC_TO
		target_fd = STDOUT_FILENO;
	dup_result = dup2(fd, target_fd);
	// Close the original fd AFTER dup2, regardless of success or failure
	// If dup2 fails, fd might still be open.
	close(fd);
	if (dup_result == -1)
	{
		// Use perror for consistency with open_redirect_file error reporting
		ft_putstr_fd(RED "minishell: ", STDERR_FILENO);
		// dup2 errors are less common, but could be EBADF, EINTR etc.
		// We can use the filename for context,
		// though dup2 itself doesn't use it.
		ft_putstr_fd(filename, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
		perror("dup2 failed"); // perror will print the specific dup2 error
		ft_putstr_fd(RESET, STDERR_FILENO);
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

	// Skip heredoc processing here, it's handled separately
	if (redir->type == TOK_HERE_DOC_FROM)
	{
		// If heredoc fd is already set (from pre-reading), dup it
		if (redir->fd >= 0)
		{
			redirect_result = redirect_std_fd(redir->fd, redir->type,
					"heredoc");
			redir->fd = -1; // Mark as used/closed
			return (redirect_result);
		}
		return (0); // No file to open for heredoc here
	}
	// Open the file for other redirection types
	fd = open_redirect_file(redir->type, redir->filename);
	if (fd == -1)
		return (-1); // Error already printed by open_redirect_file
	// Redirect stdin/stdout
	redirect_result = redirect_std_fd(fd, redir->type, redir->filename);
	// fd is closed inside redirect_std_fd
	return (redirect_result); // Return status of dup2
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
		if (result != 0) // If process_redirection failed
		{
			return (-1); // Stop processing and return error
		}
		redir = redir->next;
	}
	return (0); // All redirections succeeded
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
	// Process the list; it will stop and return -1 on the first error
	return (process_redirection_list(redirections));
}
