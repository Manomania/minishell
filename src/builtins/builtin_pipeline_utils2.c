/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_pipeline_utils2.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 18:14:22 by maximart          #+#    #+#             */
/*   Updated: 2025/04/29 12:49:59 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @brief Sets up file descriptors for a built-in in a pipeline
 *
 * @param fds Array containing [input_fd, output_fd]
 * @param saved_fds Array to store saved original fds
 * @return int 0 on success, error code otherwise
 */

#include "builtins.h"
#include "error.h"
#include "minishell.h"

int	setup_builtin_pipeline_fds(int *fds, int saved_fds[2])
{
	saved_fds[0] = dup(STDIN_FILENO);
	saved_fds[1] = dup(STDOUT_FILENO);
	if (saved_fds[0] == -1 || saved_fds[1] == -1)
	{
		if (saved_fds[0] != -1)
			close(saved_fds[0]);
		if (saved_fds[1] != -1)
			close(saved_fds[1]);
		return (ERR_IO);
	}
	if (fds[0] != STDIN_FILENO && dup2(fds[0], STDIN_FILENO) == -1)
	{
		restore_pipeline_fds(saved_fds[0], saved_fds[1]);
		return (ERR_FD);
	}
	if (fds[1] != STDOUT_FILENO && dup2(fds[1], STDOUT_FILENO) == -1)
	{
		restore_pipeline_fds(saved_fds[0], saved_fds[1]);
		return (ERR_FD);
	}
	return (0);
}

/**
 * @brief Restores all file descriptors to original state
 *
 * @param saved_fds Array with saved descriptors
 */
void	restore_all_fds(int saved_fds[2])
{
	restore_pipeline_fds(saved_fds[0], saved_fds[1]);
}
