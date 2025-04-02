/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmdas_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 15:33:08 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/30 15:18:45 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "minishell.h"

/**
 * @brief Handles the last process's status
 *
 * @param pids Array of process IDs
 * @param status Status of the process
 * @param i Current process index
 * @return int Exit status code
 */
static int	handle_last_process_status(pid_t *pids, int status, int i)
{
	int	last_status;

	last_status = 0;
	if (pids[i] > 0)
	{
		if (WIFEXITED(status))
			last_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			last_status = 128 + WTERMSIG(status);
	}
	else if (pids[i] == -2)
		last_status = 0;
	return (last_status);
}

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
	if (was_signaled && isatty(STDOUT_FILENO))
		write(STDOUT_FILENO, "\n", 1);
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
 * @brief Processes a child's wait status
 *
 * @param pid Process ID
 * @param status Pointer to status variable
 * @return int 1 if process was signaled, 0 otherwise
 */
static int	process_child_wait(pid_t pid, int *status)
{
	if (pid > 0)
	{
		waitpid(pid, status, 0);
		if (WIFSIGNALED(*status))
			return (1);
	}
	return (0);
}

/**
 * @brief Handle waiting for pipeline processes
 *
 * @param pids Array of process IDs
 * @param count Number of processes
 * @return int Exit status of the last command
 */
int	wait_for_pipeline_processes(pid_t *pids, int count)
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
		was_signaled |= process_child_wait(pids[i], &status);
		if (i == count - 1 && pids[i] > 0)
		{
			if (WIFEXITED(status))
				last_status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				last_status = 128 + WTERMSIG(status);
		}
		i++;
	}
	if (was_signaled && isatty(STDOUT_FILENO))
		write(STDOUT_FILENO, "\n", 1);
	return (last_status);
}
