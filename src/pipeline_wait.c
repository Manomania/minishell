/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_wait.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 17:34:46 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/08 17:35:01 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Waits for all commands in a pipeline to complete
 *
 * @param pids Array of process IDs
 * @param cmd_count Number of commands
 * @return int Exit status of the last command
 */
int	wait_for_commands(pid_t *pids, int cmd_count)
{
	int	i;
	int	status;
	int	exit_status;

	i = 0;
	exit_status = 0;
	while (i < cmd_count)
	{
		waitpid(pids[i], &status, 0);
		if (WIFEXITED(status))
			exit_status = WEXITSTATUS(status);
		i++;
	}
	free(pids);
	return (exit_status);
}
