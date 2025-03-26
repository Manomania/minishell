/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmdas_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 15:33:08 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/26 15:08:24 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "minishell.h"

/**
 * @brief Creates a pipe for the next command
 *
 * @param pipe_fds Array to store pipe file descriptors
 * @return int 0 on success, -1 on error
 */
int	setup_pipe(int pipe_fds[2])
{
	if (pipe(pipe_fds) == -1)
	{
		perror("pipe");
		return (-1);
	}
	fcntl(pipe_fds[0], F_SETFD, FD_CLOEXEC);
	fcntl(pipe_fds[1], F_SETFD, FD_CLOEXEC);
	return (0);
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

	i = 0;
	last_status = 0;
	while (i < count)
	{
		if (pids[i] > 0)
			waitpid(pids[i], &status, 0);
		if (i == count - 1)
		{
			if (pids[i] > 0)
			{
				if (WIFEXITED(status))
					last_status = WEXITSTATUS(status);
				else if (WIFSIGNALED(status))
					last_status = 128 + WTERMSIG(status);
			}
			else if (pids[i] == -2)
				last_status = 0;
		}
		i++;
	}
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

	i = 0;
	last_status = 0;
	while (i < count)
	{
		if (pids[i] > 0)
			waitpid(pids[i], &status, 0);
		if (i == count - 1 && pids[i] > 0)
		{
			if (WIFEXITED(status))
				last_status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				last_status = 128 + WTERMSIG(status);
		}
		i++;
	}
	return (last_status);
}
