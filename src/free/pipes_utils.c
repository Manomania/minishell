/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 10:24:52 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/13 12:09:46 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Closes all pipe file descriptors if they are valid
 *
 * @param pipes Array of pipe file descriptors
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

/**
 * @brief Closes only the previous pipe file descriptors
 *
 * @param pipes Array of pipe file descriptors
 * @param cmd_index Current command index
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
