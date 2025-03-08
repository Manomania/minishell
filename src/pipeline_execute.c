/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_execute.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 17:41:30 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/08 18:05:17 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Executes a pipeline of commands
 *
 * @param ctx Context
 * @param pipeline Pipeline structure containing commands
 * @return int Exit status of the last command
 */
int	pipeline_execute(t_ctx *ctx, t_pipeline *pipeline)
{
	int			i;
	int			cmd_count;
	int			pipes[2][2];
	pid_t		*pids;
	t_command	*current;

	cmd_count = count_commands(pipeline->commands);
	pids = malloc(sizeof(pid_t) * cmd_count);
	if (!pids)
		return (ctx_error(ERR_ALLOC));
	i = 0;
	current = pipeline->commands;
	pipes[0][0] = -1;
	pipes[0][1] = -1;
	pipes[1][0] = -1;
	pipes[1][1] = -1;
	while (current)
	{
		if (create_pipes(pipes, i, cmd_count) == -1)
			return (free(pids), ctx_error(ERR_IO_ERROR));
		pids[i] = fork();
		if (pids[i] == -1)
			return (free(pids), ctx_error(ERR_IO_ERROR));
		if (pids[i] == 0)
			execute_command_in_pipeline(ctx, current, pipes, i, cmd_count);
		close_previous_pipe(pipes, i);
		swap_pipes(pipes);
		current = current->next;
		i++;
	}
	close_all_pipes(pipes);
	return (wait_for_commands(pids, cmd_count));
}
