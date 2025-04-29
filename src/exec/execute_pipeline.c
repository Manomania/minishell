/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipeline.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 12:37:45 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/29 13:38:49 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "error.h"
#include "minishell.h"

/**
 * @brief Executes all commands in a pipeline
 *
 * @param ctx Context containing shell state
 * @param pids Array to store process IDs
 * @param cmd_count Total number of commands in pipeline
 * @return int 0 on success, -1 on error
 */
static int	execute_pipeline_commands(t_ctx *ctx, pid_t *pids, int cmd_count)
{
	t_command	*cmd;
	int			i;
	int			prev_pipe;
	int			pipe_fds[2];
	int			result;

	cmd = ctx->cmd;
	prev_pipe = STDIN_FILENO;
	i = 0;
	while (cmd && i < cmd_count)
	{
		if (i < cmd_count - 1)
		{
			if (setup_pipe(pipe_fds) == -1)
			{
				if (prev_pipe != STDIN_FILENO)
					close(prev_pipe);
				return (-1);
			}
		}
		else
		{
			pipe_fds[0] = -1;
			pipe_fds[1] = STDOUT_FILENO;
		}
		result = process_pipeline_command(ctx, cmd, prev_pipe, pipe_fds, pids,
				i);
		if (result == -1)
			return (-1);
		if (i < cmd_count - 1)
			prev_pipe = pipe_fds[0];
		cmd = cmd->next;
		i++;
	}
	return (0);
}

/**
 * @brief Waits for all pipeline processes and processes their exit status
 *
 * @param pids Array of process IDs
 * @param cmd_count Number of commands in pipeline
 * @param ctx Shell context
 * @return int Exit status of the last command
 */
static int	wait_for_pipeline_processes(pid_t *pids, int cmd_count, t_ctx *ctx)
{
	int	i;
	int	status;
	int	last_status;
	int	was_signaled;

	was_signaled = 0;
	last_status = 0;
	i = 0;
	while (i < cmd_count)
	{
		if (pids[i] > 0)
		{
			waitpid(pids[i], &status, 0);
			if (WIFSIGNALED(status))
			{
				was_signaled = 1;
				if (i == cmd_count - 1)
					last_status = 128 + WTERMSIG(status);
			}
			else if (WIFEXITED(status) && i == cmd_count - 1)
				last_status = WEXITSTATUS(status);
		}
		else if (pids[i] == -2 && i == cmd_count - 1)
			last_status = ctx->exit_status;
		i++;
	}
	if (was_signaled)
	{
		if (WTERMSIG(status) == SIGQUIT)
			ft_printf_fd(STDERR_FILENO, "Quit (core dumped)\n");
		else if (WTERMSIG(status) == SIGINT && isatty(STDOUT_FILENO))
			write(STDOUT_FILENO, "\n", 1);
	}
	return (last_status);
}

/**
 * @brief Executes a pipeline of commands
 *
 * @param ctx Context containing shell state
 * @return int Exit status of the last command in the pipeline
 */
int	execute_pipeline(t_ctx *ctx)
{
	int		cmd_count;
	pid_t	*pids;
	int		status;

	cmd_count = count_commands(ctx->cmd);
	pids = malloc(sizeof(pid_t) * cmd_count);
	if (!pids)
		return (error(NULL, "pipeline", ERR_ALLOC));
	prepare_all_pipeline_files(ctx->cmd);
	setup_parent_signals();
	if (execute_pipeline_commands(ctx, pids, cmd_count) != 0)
	{
		free(pids);
		setup_signals();
		return (1);
	}
	status = wait_for_pipeline_processes(pids, cmd_count, ctx);
	free(pids);
	setup_signals();
	return (status);
}
