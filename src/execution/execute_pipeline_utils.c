/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipeline_utils.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 15:50:54 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/05 16:15:56 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"

/**
 * @brief Counts commands in a pipeline
 *
 * @param cmd Command list
 * @return int Number of commands
 */
int	count_commands(t_command *cmd)
{
	t_command	*current;
	int			count;

	count = 0;
	current = cmd;
	while (current)
	{
		count++;
		current = current->next;
	}
	return (count);
}

/**
 * @brief Setup pipe for command
 *
 * Creates pipe if needed or flags error
 *
 * @param pipe_fds Pipe file descriptors array
 * @param has_next Whether current command has next
 * @return int 1 on success, 0 on failure
 */
int	setup_command_pipe(int pipe_fds[2], t_bool has_next)
{
	if (has_next)
	{
		if (pipe(pipe_fds) == -1)
		{
			error("pipe", NULL, ERR_PIPE);
			return (0);
		}
	}
	return (1);
}
