/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipeline_fork.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 15:54:01 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/05 16:08:56 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "execute.h"
#include "minishell.h"

/**
 * @brief Execute child process
 *
 * Handles child process execution with proper fd redirections
 *
 * @param ctx Shell context
 * @param a Arguments
 */
static void	execute_child_command(t_ctx *ctx, t_ecc_args a)
{
	int		out_fd;
	t_fds	fds;

	if (a.cmd->next)
	{
		close(a.pipe_fds[0]);
		out_fd = a.pipe_fds[1];
	}
	else
		out_fd = -1;
	fds.in = a.prev_pipe;
	fds.out = out_fd;
	execute_command_in_child(ctx, a.cmd, fds, a.pids);
}

/**
 * @brief Create and execute single fork
 *
 * Creates fork and executes appropriate code
 *
 * @param ctx Shell context
 * @param a Arguments
 * @return int 1 on success, 0 on error
 */
int	handle_command_fork(t_ctx *ctx, t_hcf_args a)
{
	a.pids[a.i] = fork();
	if (a.pids[a.i] == -1)
	{
		handle_fork_error(a.pids, count_commands(a.current), a.pipe_fds,
			a.prev_pipe);
		return (0);
	}
	if (a.pids[a.i] == 0)
		execute_child_command(ctx, (t_ecc_args){a.current, a.prev_pipe,
			a.pipe_fds, a.pids});
	return (1);
}

/**
 * @brief Handle fork error cleanup
 *
 * Kills all created processes and cleans up on fork error
 *
 * @param pids Process ids array
 * @param cmd_count Number of commands
 * @param pipe_fds Current pipe file descriptors
 * @param prev_pipe Previous pipe read fd
 */
void	handle_fork_error(pid_t *pids, int cmd_count, int pipe_fds[2],
		int prev_pipe_read)
{
	int	j;

	j = 0;
	while (j < cmd_count && pids[j] > 0)
		kill(pids[j++], SIGKILL);
	free(pids);
	if (pipe_fds[0] != -1)
	{
		close(pipe_fds[0]);
		close(pipe_fds[1]);
	}
	if (prev_pipe_read != -1)
		close(prev_pipe_read);
}
