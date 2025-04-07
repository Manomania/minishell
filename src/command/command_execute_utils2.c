/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_execute_utils2.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 15:52:23 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/07 20:29:43 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "debug.h"
#include "error.h"
#include "minishell.h"

static void	yes(t_ctx *ctx, int pid)
{
	reset_signals();
	if (setup_heredocs(ctx, ctx->cmd) != 0)
	{
		ctx_clear(ctx);
		exit(EXIT_FAILURE);
	}
	if (setup_redirections(ctx->cmd->redirection) != 0)
	{
		ctx_clear(ctx);
		exit(EXIT_FAILURE);
	}
	ctx_clear(ctx);
	exit(EXIT_SUCCESS);
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
		yes(ctx, pid);
	waitpid(pid, &status, 0);
	if (WIFSIGNALED(status))
		was_signaled = 1;
	setup_signals();
	if (was_signaled && isatty(STDOUT_FILENO))
		write(STDOUT_FILENO, "\n", 1);
	return (get_exit_status(status));
}

/**
 * @brief Clean up resources before child process exit
 *
 * @param ctx Context containing all resources
 */
void	cleanup_child_process(t_ctx *ctx)
{
	cleanup_heredoc_resources(ctx);
	if (ctx->tokens)
	{
		free_all_token(ctx->tokens);
		ctx->tokens = NULL;
	}
	ctx_clear(ctx);
}
