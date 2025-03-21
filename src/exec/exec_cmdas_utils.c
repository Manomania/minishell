/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmdas_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 15:33:08 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/21 10:22:06 by elagouch         ###   ########.fr       */
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
		if (pids[i] > 0) // Skip built-ins (which have pid = -2)
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
			else if (pids[i] == -2) // This was a built-in
				last_status = 0;
		}
		i++;
	}
	return (last_status);
}
