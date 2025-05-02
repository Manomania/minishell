/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_redirection_utils.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 15:22:36 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/02 14:04:33 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"

/**
 * @brief Executes the child process for redirection-only commands
 *
 * @param ctx Context containing environment info
 * @return void (does not return)
 */
static void	execute_redirection_child(t_ctx *ctx)
{
	reset_signals();
	if (setup_heredocs(ctx, ctx->cmd) != 0)
	{
		ctx_clear(ctx);
		exit(EXIT_FAILURE);
	}
	if (setup_redirections(ctx->cmd->redirection, ctx) != 0)
	{
		ctx_clear(ctx);
		exit(EXIT_FAILURE);
	}
	ctx_clear(ctx);
	if (ctx->exit_status != 0)
		exit(ctx->exit_status);
	exit(EXIT_SUCCESS);
}

/**
 * @brief Handles the parent process for redirection commands
 *
 * @param pid Process ID of the child
 * @return int Exit status (0 for success, non-zero on error)
 */
static int	handle_redirection_parent(pid_t pid)
{
	int	status;

	waitpid(pid, &status, 0);
	setup_signals();
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
	{
		if (WTERMSIG(status) == SIGINT && isatty(STDOUT_FILENO))
			write(STDOUT_FILENO, "\n", 1);
		return (128 + WTERMSIG(status));
	}
	return (0);
}

/**
 * @brief Handles redirect-only commands (like "> file")
 * Preserves error status from redirection operations
 *
 * @param ctx Context containing environment info
 * @return int Exit status (0 for success, non-zero on error)
 */
int	execute_redirections_only(t_ctx *ctx)
{
	pid_t	pid;

	setup_parent_signals();
	pid = fork();
	if (pid == -1)
		return (error(NULL, "execute_redirections_only", ERR_CHILD));
	if (pid == 0)
		execute_redirection_child(ctx);
	return (handle_redirection_parent(pid));
}

/**
 * @brief Creates files for all output redirections in pipeline
 *
 * @param cmd First command in the pipeline
 * @return int 0 on success
 */
int	prepare_all_pipeline_files(t_command *cmd)
{
	t_command		*current;
	t_redirection	*redir;
	int				fd;

	current = cmd;
	while (current)
	{
		redir = current->redirection;
		while (redir)
		{
			if (redir->type == TOK_REDIR_TO || redir->type == TOK_HERE_DOC_TO)
			{
				fd = open_redirection_file(redir);
				if (fd != -1)
					close(fd);
			}
			redir = redir->next;
		}
		current = current->next;
	}
	return (0);
}
