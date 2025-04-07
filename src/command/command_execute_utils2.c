/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_execute_utils2.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 18:48:22 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/07 18:57:44 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "debug.h"
#include "error.h"
#include "minishell.h"

/**
 * @brief Checks if the command has only redirections and no command
 *
 * @param cmd Command to check
 * @return true if command has only redirections, false otherwise
 */
t_bool	has_only_redirections(t_command *cmd)
{
	return (cmd && cmd->redirection && (!cmd->args || !cmd->args[0]));
}

/**
 * @brief Executes a command that consists of only redirections
 *
 * This handles the case of standalone redirections like "<<EOF" without a
 * command
 *
 * @param ctx Context containing environment information
 * @return int Exit status (0 for success, non-zero on error)
 */
int	execute_redirections_only(t_ctx *ctx)
{
	pid_t	pid;
	int		status;
	int		was_signaled;

	was_signaled = 0;
	setup_parent_signals();
	pid = fork();
	if (pid == -1)
		return (error(NULL, "execute_redirections_only", ERR_CHILD));
	if (pid == 0)
	{
		reset_signals();
		if (setup_heredocs(ctx, ctx->cmd) != 0)
			exit(EXIT_FAILURE);
		if (setup_redirections(ctx->cmd->redirection) != 0)
			exit(EXIT_FAILURE);
		exit(EXIT_SUCCESS);
	}
	waitpid(pid, &status, 0);
	if (WIFSIGNALED(status))
		was_signaled = 1;
	setup_signals();
	if (was_signaled && isatty(STDOUT_FILENO))
		write(STDOUT_FILENO, "\n", 1);
	return (get_exit_status(status));
}
