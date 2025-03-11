/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmdas.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/30 16:37:25 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/11 10:31:07 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Nullify pipes
 *
 * @param pipe_prev Previous pipe
 * @param pipe_curr Current pipe
 */
static void	null_pipes(int *pipe_prev, int *pipe_curr)
{
	pipe_prev[0] = -1;
	pipe_prev[1] = -1;
	pipe_curr[0] = -1;
	pipe_curr[1] = -1;
}

/**
 * @brief Executes a command with arguments: parent POV
 *
 * @param current_cmd Current command
 * @param pipe_prev Previous pipe
 * @param pipe_curr Current pipe
 */
void	exec_cmda_parent(t_command **current_cmd, int *pipe_prev,
		int *pipe_curr)
{
	if (pipe_prev[0] != -1)
	{
		close(pipe_prev[0]);
		close(pipe_prev[1]);
	}
	if ((*current_cmd)->next)
	{
		pipe_prev[0] = pipe_curr[0];
		pipe_prev[1] = pipe_curr[1];
	}
	*current_cmd = (*current_cmd)->next;
}

/**
 * @brief Main "loop" for commands execution
 *
 * @param ctx Context
 * @param current_cmd Current command
 * @param pipe_prev Previous pipe
 * @param pipe_curr Current pipe
 */
static void	main_exec_loop(t_ctx *ctx, t_command *current_cmd, int *pipe_prev,
		int *pipe_curr)
{
	pid_t	pid;

	while (current_cmd)
	{
		if (current_cmd->next)
		{
			if (pipe(pipe_curr) == -1)
				ctx_error(ERR_PIPE);
		}
		pid = fork();
		if (pid == -1)
			ctx_error(ECHILD);
		if (pid == 0)
			exec_cmda_child(ctx, current_cmd, pipe_prev, pipe_curr);
		else
			exec_cmda_parent(&current_cmd, pipe_prev, pipe_curr);
	}
}

/**
 * @brief Execute the commands
 *
 * @param ctx Context
 */
void	exec_cmdas(t_ctx *ctx)
{
	t_command	*current_cmd;
	int			pipe_prev[2];
	int			pipe_curr[2];

	current_cmd = ctx->cmd;
	null_pipes(pipe_prev, pipe_curr);
	main_exec_loop(ctx, current_cmd, pipe_prev, pipe_curr);
	if (ctx->fd_file_in != -1)
		close(ctx->fd_file_in);
	if (ctx->fd_file_out != -1)
		close(ctx->fd_file_out);
	while (wait(NULL) > 0)
		;
}
