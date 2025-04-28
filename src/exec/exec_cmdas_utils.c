/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmdas_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 15:33:08 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/28 13:31:42 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "minishell.h"

/**
 * @brief Waits for all child processes and reports status
 *
 * @param pids Array of process IDs
 * @param count Number of processes
 * @return int Exit status of the last command
 */
int	wait_for_pids(pid_t *pids, int count)
{
	int	i;
	int	status;
	int	last_status;
	int	was_signaled;

	was_signaled = 0;
	i = 0;
	last_status = 0;
	while (i < count)
	{
		if (pids[i] > 0)
		{
			waitpid(pids[i], &status, 0);
			if (WIFSIGNALED(status))
				was_signaled = 1;
		}
		if (i == count - 1)
			last_status = handle_last_process_status(pids, status, i);
		i++;
	}
	if (was_signaled)
		handle_signal_output(status);
	return (last_status);
}

/**
 * @brief Initialize pipe data structure
 *
 * @param data Pipe data structure to initialize
 * @param ctx Context with command info
 * @return t_bool TRUE on success, FALSE on failure
 */
t_bool	init_pipe_data(t_pipe_data *data, t_ctx *ctx)
{
	data->current = ctx->cmd;
	data->cmd_count = count_commands(data->current);
	data->i = 0;
	data->prev_pipe = STDIN_FILENO;
	data->pids = malloc(sizeof(pid_t) * (size_t)data->cmd_count);
	if (!data->pids)
		return (false);
	return (true);
}

/**
 * Process the status of the last command in the pipeline.
 *
 * @param status The status from waitpid.
 * @param was_signaled Pointer to flag indicating if a signal was received.
 * @return int Processed exit status.
 */
int	process_last_command_status(int status, int *was_signaled)
{
	int	result;

	result = 0;
	if (WIFSIGNALED(status))
	{
		*was_signaled = 1;
		result = 128 + WTERMSIG(status);
	}
	else if (WIFEXITED(status))
		result = WEXITSTATUS(status);
	return (result);
}

/**
 * Handle the status for the last command based on its pid value.
 *
 * @param pid Process ID or special status indicator.
 * @param status Current status from waitpid.
 * @param was_signaled Pointer to flag indicating if a signal was received.
 * @param ctx Shell context.
 * @return int Final status for this command.
 */
int	handle_last_command_status(pid_t pid, int status, int *was_signaled,
		t_ctx *ctx)
{
	int	last_status;

	last_status = 0;
	if (pid > 0)
		last_status = process_last_command_status(status, was_signaled);
	else if (pid == -2)
		last_status = ctx->exit_status;
	else if (pid == -1)
		last_status = 127;
	return (last_status);
}

/**
 * Handle waiting for pipeline processes and determine final status.
 *
 * @param pids Array of process IDs or status indicators (-1, -2).
 * @param count Number of commands in the pipeline.
 * @param ctx Shell context (needed for builtin status).
 * @return int Exit status of the last command in the pipeline.
 */
int	wait_for_pipeline_processes(pid_t *pids, int count, t_ctx *ctx)
{
	int	i;
	int	status;
	int	last_status;
	int	was_signaled;

	i = 0;
	status = 0;
	last_status = 0;
	was_signaled = 0;
	if (count <= 0)
		return (0);
	while (i < count)
	{
		if (pids[i] > 0)
		{
			waitpid(pids[i], &status, 0);
			if (WIFSIGNALED(status))
				was_signaled = 1;
		}
		if (i == count - 1)
			last_status = handle_last_command_status(pids[i], status,
					&was_signaled, ctx);
		i++;
	}
	return (last_status);
}
