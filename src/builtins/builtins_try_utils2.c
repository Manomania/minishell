/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_try_utils2.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 15:55:45 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/15 15:58:11 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "error.h"
#include "minishell.h"

/**
 * Handles builtin redirection and execution process
 *
 * @param ctx Context for shell environment
 * @param cmd Command to execute
 * @param exit_status Pointer to store exit status
 * @param saved_fds Array with original file descriptors
 * @return t_bool true if successful execution, false otherwise
 */
static t_bool	process_builtin(t_ctx *ctx, t_command *cmd, int *exit_status,
		int saved_fds[2])
{
	int	redir_status;

	redir_status = setup_builtin_redirections(cmd, saved_fds);
	if (redir_status == -1)
	{
		*exit_status = 1;
		builtin_restore_redirections(saved_fds);
		return (1);
	}
	run_builtin_command(ctx, cmd, exit_status);
	builtin_restore_redirections(saved_fds);
	return (1);
}

/**
 * Pre-creates any output files for redirections
 *
 * @param cmd Command with redirections
 * @return 0 on success, -1 on failure
 */
static int	prepare_builtin_files(t_command *cmd)
{
	t_redirection	*redir;
	int				fd;

	redir = cmd->redirection;
	while (redir)
	{
		if (redir->type == TOK_REDIR_TO || redir->type == TOK_HERE_DOC_TO)
		{
			fd = open_redirection_file(redir);
			if (fd != -1)
			{
				if (redir->type == TOK_REDIR_TO)
					write(fd, "", 0);
				close(fd);
			}
			else
				return (-1);
		}
		redir = redir->next;
	}
	return (0);
}

/**
 * Sets up saved file descriptors for redirection
 *
 * @param saved_fds Array to store original file descriptors
 * @return t_bool true if successful, false on error
 */
static t_bool	save_original_fds(int saved_fds[2])
{
	saved_fds[0] = dup(STDIN_FILENO);
	saved_fds[1] = dup(STDOUT_FILENO);
	if (saved_fds[0] == -1 || saved_fds[1] == -1)
	{
		if (saved_fds[0] != -1)
			close(saved_fds[0]);
		if (saved_fds[1] != -1)
			close(saved_fds[1]);
		(void)error(NULL, "save_original_fds", ERR_IO);
		return (0);
	}
	return (1);
}

/**
 * Executes a built-in command with proper redirection handling
 *
 * @param ctx Context for shell environment
 * @param cmd Command to execute
 * @param exit_status Pointer to store exit status
 * @return t_bool true if a built-in was executed, false otherwise
 */
t_bool	execute_builtin(t_ctx *ctx, t_command *cmd, int *exit_status)
{
	int	saved_fds[2];

	if (prepare_builtin_files(cmd) == -1)
	{
		*exit_status = 1;
		return (1);
	}
	if (!save_original_fds(saved_fds))
		return (0);
	return (process_builtin(ctx, cmd, exit_status, saved_fds));
}
