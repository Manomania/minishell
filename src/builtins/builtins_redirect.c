/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_redirect.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 09:52:58 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/15 14:41:28 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "debug.h"
#include "error.h"
#include "minishell.h"

// Forward declaration from builtins_redirect_utils.c if needed,
// or include header
int		apply_redirection(t_redirection *redir);

/**
 * @brief Opens a file for redirection (shared logic)
 *
 * @param redir Redirection information
 * @return int File descriptor or -1 on error
 */
int	open_redirection_file(t_redirection *redir)
{
	int	fd;

	int flags; // Use flags for clarity
	fd = -1;
	if (redir->type == TOK_REDIR_FROM)
		flags = O_RDONLY;
	else if (redir->type == TOK_REDIR_TO)
		flags = O_WRONLY | O_CREAT | O_TRUNC;
	else if (redir->type == TOK_HERE_DOC_TO)
		flags = O_WRONLY | O_CREAT | O_APPEND;
	else if (redir->type == TOK_HERE_DOC_FROM)
	{
		// If heredoc fd is pre-opened, return it directly
		if (redir->fd >= 0)
			return (redir->fd);
		else // Should not happen if heredocs are pre-processed correctly
			return (-1);
	}
	else // Should not happen
		return (-1);
	fd = open(redir->filename, flags, 0644); // Use flags
	if (fd == -1)
	{
		// Print error using perror for consistency
		ft_putstr_fd(RED "minishell: ", STDERR_FILENO);
		perror(redir->filename);
		ft_putstr_fd(RESET, STDERR_FILENO);
	}
	return (fd);
}

/**
 * @brief Sets up redirections for a built-in command. Stops on first error.
 *
 * @param cmd Command with redirections

	* @param saved_fds Array to store original file descriptors (may not be needed here anymore)
 * @return int 0 on success, -1 on error
 */
int	builtin_setup_redirections(t_command *cmd, int saved_fds[2])
{
	t_redirection	*redir;
	int				result;

	(void)saved_fds;
	// Mark as unused if not strictly needed for saving/restoring within this specific function anymore
	redir = cmd->redirection;
	while (redir)
	{
		// Heredocs are handled by setup_heredocs before command execution
		if (redir->type == TOK_HERE_DOC_FROM)
		{
			// If heredoc fd is present, apply it
			if (redir->fd >= 0)
			{
				if (apply_input_redirection(redir->fd) == -1)
				{
					// Error applying heredoc redirection
					return (-1);
				}
				redir->fd = -1; // Mark as used/closed
			}
			// Otherwise, skip (should have been handled)
			redir = redir->next;
			continue ;
		}
		// Apply other types of redirections
		result = apply_redirection(redir);
		// apply_redirection handles open and dup2
		if (result == -1)
		{
			// Error occurred and was printed by apply_redirection/open_redirection_file
			return (-1); // Stop processing further redirections
		}
		redir = redir->next;
	}
	return (0); // All redirections succeeded
}

/**
 * @brief Restores original standard file descriptors after built-in execution
 * This should be called *after* the builtin command finishes.
 *
 * @param saved_fds Array containing saved original stdin and stdout FDs
 * @return void
 */
void	builtin_restore_redirections(int saved_fds[2])
{
	// Restore stdin if it was saved
	if (saved_fds[0] != -1)
	{
		// Check for errors, though unlikely if saved_fds[0] was valid
		if (dup2(saved_fds[0], STDIN_FILENO) == -1)
		{
			perror("minishell: dup2 failed restoring stdin");
		}
		close(saved_fds[0]); // Close the saved descriptor
		saved_fds[0] = -1;   // Mark as closed
	}
	// Restore stdout if it was saved
	if (saved_fds[1] != -1)
	{
		// Check for errors
		if (dup2(saved_fds[1], STDOUT_FILENO) == -1)
		{
			perror("minishell: dup2 failed restoring stdout");
		}
		close(saved_fds[1]); // Close the saved descriptor
		saved_fds[1] = -1;   // Mark as closed
	}
}
