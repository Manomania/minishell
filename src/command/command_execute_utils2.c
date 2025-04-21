/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_execute_utils2.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 15:52:23 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/08 14:50:31 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"

/**
 * @brief Reads heredoc content for all commands in a pipeline
 *
 * This function processes all heredocs in a command pipeline before
 * any command execution begins. It stores the file descriptors in
 * the redirection structures.
 *
 * @param ctx Context containing environment information
 * @return 0 on success, -1 on error
 */
int	read_all_heredocs(t_ctx *ctx)
{
	t_command		*current;
	t_redirection	*redir;
	int				heredoc_fd;

	current = ctx->cmd;
	while (current)
	{
		redir = current->redirection;
		while (redir)
		{
			if (redir->type == TOK_HERE_DOC_FROM)
			{
				heredoc_fd = create_heredoc(ctx, redir->filename);
				if (heredoc_fd == -1)
					return (-1);
				redir->fd = heredoc_fd;
			}
			redir = redir->next;
		}
		current = current->next;
	}
	return (0);
}

/**
 * @brief Sets up redirections in child process for command with only
 * redirections
 *
 * @param ctx Context containing environment information
 */
static void	setup_stuff(t_ctx *ctx)
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
		setup_stuff(ctx);
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
