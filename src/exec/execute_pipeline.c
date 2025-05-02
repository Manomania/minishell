/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipeline.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 12:37:45 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/02 13:58:34 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "error.h"
#include "minishell.h"

/**
 * @brief Condition to be used in execute_pipeline_commands
 *
 * @param i Index
 * @param cmd_count Commands count
 * @param pipe_fds Pipes file descriptors
 * @param prev_pipe Previous pipe fd
 * @return t_bool true if should exit function, false otherwise
 */
static t_bool	execute_pipeline_commands_condition(int i, int cmd_count,
		int *pipe_fds, int prev_pipe)
{
	if (i < cmd_count - 1)
	{
		if (setup_pipe(pipe_fds) == -1)
		{
			if (prev_pipe != STDIN_FILENO)
				close(prev_pipe);
			return (true);
		}
	}
	else
	{
		pipe_fds[0] = -1;
		pipe_fds[1] = STDOUT_FILENO;
	}
	return (false);
}

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
		if (execute_pipeline_commands_condition(i, cmd_count, pipe_fds,
				prev_pipe))
			return (-1);
		result = process_pipeline_command(ctx, (t_ppca){cmd, prev_pipe,
				pipe_fds, pids, i});
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
 * @brief Main loop when running a process in a pipeline
 *
 * @param ctx Context
 * @param a Arguments list
 * @return t_bool Whether the process was signaled or not
 */
static t_bool	pipeline_process_loop(t_ctx *ctx,
		t_pipeline_process_loop_args a)
{
	int	was_signaled;

	was_signaled = false;
	if (a.pid > 0)
	{
		waitpid(a.pid, a.status, 0);
		if (WIFSIGNALED(*a.status))
		{
			was_signaled = true;
			if (*a.i == a.cmd_count - 1)
				*a.last_status = 128 + WTERMSIG(*a.status);
		}
		else if (WIFEXITED(*a.status) && *a.i == a.cmd_count - 1)
			*a.last_status = WEXITSTATUS(*a.status);
	}
	else if (a.pid == -2 && *a.i == a.cmd_count - 1)
		*a.last_status = ctx->exit_status;
	*a.i += 1;
	return (was_signaled);
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
	int		i;
	int		status;
	int		last_status;
	t_bool	was_signaled;

	was_signaled = false;
	last_status = 0;
	i = 0;
	while (i < cmd_count)
		if (pipeline_process_loop(ctx, (t_pipeline_process_loop_args){pids[i],
				&i, &status, cmd_count, &last_status}))
			was_signaled = true;
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
