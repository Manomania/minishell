/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 17:33:36 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/08 17:57:05 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Creates pipes for command execution
 *
 * @param pipes Array of pipe file descriptors
 * @param cmd_index Index of the current command
 * @param cmd_count Total number of commands
 * @return int 0 on success, -1 on error
 */
int	create_pipes(int pipes[2][2], int cmd_index, int cmd_count)
{
	if (cmd_index < cmd_count - 1)
	{
		if (pipe(pipes[1]) == -1)
			return (-1);
	}
	return (0);
}

/**
 * @brief Closes previous pipe file descriptors if they are valid
 *
 * @param pipes Array of pipe file descriptors
 * @param cmd_index Index of the current command
 * @return void
 */
void	close_previous_pipe(int pipes[2][2], int cmd_index)
{
	if (cmd_index > 0)
	{
		if (pipes[0][0] >= 0)
			close(pipes[0][0]);
		if (pipes[0][1] >= 0)
			close(pipes[0][1]);
	}
}

/**
 * @brief Swaps pipe arrays for next command
 *
 * @param pipes Array of pipe file descriptors
 * @return void
 */
void	swap_pipes(int pipes[2][2])
{
	pipes[0][0] = pipes[1][0];
	pipes[0][1] = pipes[1][1];
}

/**
 * @brief Closes all pipe file descriptors if they are valid
 *
 * @param pipes Array of pipe file descriptors
 * @return void
 */
void	close_all_pipes(int pipes[2][2])
{
	if (pipes[0][0] >= 0)
		close(pipes[0][0]);
	if (pipes[0][1] >= 0)
		close(pipes[0][1]);
	if (pipes[1][0] >= 0)
		close(pipes[1][0]);
	if (pipes[1][1] >= 0)
		close(pipes[1][1]);
}
