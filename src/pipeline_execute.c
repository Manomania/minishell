/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_execute.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 17:41:30 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/08 18:26:21 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Initialize pipe file descriptors
 *
 * @param pipes Array of pipe file descriptors to initialize
 */
static void	init_pipes(int pipes[2][2])
{
	pipes[0][0] = -1;
	pipes[0][1] = -1;
	pipes[1][0] = -1;
	pipes[1][1] = -1;
}

/**
 * @brief Create and set up all pipes needed for command execution
 *
 * @param pids Array of process IDs
 * @param cmd_count Number of commands
 * @param pipes Array of pipe file descriptors
 * @return int 0 on success, -1 on error
 */
static int	prepare_pipeline(pid_t **pids, int cmd_count, int pipes[2][2])
{
	*pids = malloc(sizeof(pid_t) * cmd_count);
	if (!(*pids))
		return (ctx_error(ERR_ALLOC));
	init_pipes(pipes);
	return (0);
}

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
	if (prepare_pipeline(&pids, cmd_count, pipes) != 0)
		return (-1);
	i = 0;
	current = pipeline->commands;
	while (current)
	{
		if (create_pipes(pipes, i, cmd_count) == -1)
		{
			close_all_pipes(pipes);
			free(pids);
			return (ctx_error(ERR_IO_ERROR));
		}
		pids[i] = fork();
		if (pids[i] == -1)
		{
			close_all_pipes(pipes);
			free(pids);
			return (ctx_error(ERR_IO_ERROR));
		}
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
