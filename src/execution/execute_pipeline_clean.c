/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipeline_clean.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 15:47:16 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/05 15:48:24 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Cleans up pipe file descriptors
 *
 * @param prev_pipe_read Previous pipe read fd
 * @param pipe_fds Current pipe fds
 * @param has_next Whether there's a next command
 */
void	cleanup_pipes(int prev_pipe_read, int pipe_fds[2], int has_next)
{
	if (prev_pipe_read != -1)
		close(prev_pipe_read);
	if (has_next)
	{
		close(pipe_fds[1]);
		prev_pipe_read = pipe_fds[0];
	}
}

/**
 * @brief Cleanup and exit from child process
 *
 * Clears context, frees resources and exits with status
 *
 * @param ctx Shell context
 * @param pids Process ids pointer to free
 * @param status Exit status
 */
void	child_process_exit(t_ctx *ctx, int *pids, int status)
{
	ctx_clear(ctx);
	free(pids);
	exit(status);
}
