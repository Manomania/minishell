/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_redirect.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 09:52:58 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/02 14:06:43 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "error.h"
#include "minishell.h"

/**
 * @brief Sets up redirections for a built-in command. Stops on first error.
 *
 * @param ctx Context
 * @param cmd Command with redirections
 * @param saved_fds Array to store original file descriptors (may not be needed
 * here anymore)
 * @return int 0 on success, -1 on error
 */
int	builtin_setup_redirections(t_ctx *ctx, t_command *cmd, int saved_fds[2])
{
	t_redirection	*redir;
	int				result;

	(void)saved_fds;
	redir = cmd->redirection;
	while (redir)
	{
		if (redir->type == TOK_HERE_DOC_FROM)
		{
			if (redir->fd >= 0)
			{
				if (apply_input_redirection(redir->fd) == -1)
					return (-1);
				redir->fd = -1;
			}
			redir = redir->next;
			continue ;
		}
		result = apply_redirection(redir, ctx);
		if (result == -1)
			return (-1);
		redir = redir->next;
	}
	return (0);
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
	if (saved_fds[0] >= 0)
	{
		if (dup2(saved_fds[0], STDIN_FILENO) == -1)
			perror("minishell: dup2 failed restoring stdin");
		close(saved_fds[0]);
		saved_fds[0] = -1;
	}
	if (saved_fds[1] >= 0)
	{
		if (dup2(saved_fds[1], STDOUT_FILENO) == -1)
			perror("minishell: dup2 failed restoring stdout");
		close(saved_fds[1]);
		saved_fds[1] = -1;
	}
}
