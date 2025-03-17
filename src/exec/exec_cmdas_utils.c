/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmdas_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 15:33:08 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/14 15:52:37 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Executes a single command in the pipeline
 *
 * @param ctx Context with environment
 * @param cmd Current command to execute
 * @param input_fd Input file descriptor
 * @param output_fd Output file descriptor
 * @return pid_t Process ID of the child
 */
pid_t	exec_piped_command(t_ctx *ctx, t_command *cmd, int input_fd,
		int output_fd)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return (-1);
	}
	if (pid == 0)
		setup_child_process(ctx, cmd, input_fd, output_fd);
	return (pid);
}

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
