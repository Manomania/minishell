/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmdas_utils7.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 19:18:09 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/28 13:27:17 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Setup child process redirection for pipeline
 *
 * @param ctx Context
 * @param current Current command structure
 * @param prev_pipe Previous pipe read end
 * @param next_write Next pipe write end
 * @return 0 on success, -1 on error
 */
int	setup_child_redirections_bruh(t_ctx *ctx, t_command *current, int prev_pipe,
		int next_write)
{
	int	result;

	result = setup_child_pipeline_redirections(current, prev_pipe, next_write);
	if (result != 0)
		return (-1);
	if (setup_heredocs(ctx, current) != 0)
		return (-1);
	return (0);
}

/**
 * Execute command in child process and handle exit
 *
 * @param ctx Context information
 * @param current Current command structure
 * @param pipe_read_end The read end of the current pipe (to be closed)
 */
void	handle_child_process(t_ctx *ctx, t_command *current, int pipe_read_end)
{
	reset_signals();
	if (pipe_read_end > 2)
		close(pipe_read_end);
	if (!current->args || !current->args[0])
	{
		cleanup_child_process(ctx);
		exit(EXIT_SUCCESS);
	}
	execute_command(ctx, current);
	cleanup_child_process(ctx);
	exit(127);
}

/**
 * Handle parent process pipe descriptors after forking a child.
 * Closes the necessary ends of the previous and current pipes in the parent.
 *
 * @param prev_pipe Previous pipe read end descriptor.
 * @param pipe_fds Current pipe file descriptors [read, write].
 * @param i Current command index.
 * @param cmd_count Total command count.
 * @return The read end fd of the *current* pipe, to be used as prev_pipe
 * for the next iteration. Returns STDIN_FILENO if it's the last command.
 */
int	update_parent_pipes(int prev_pipe, int *pipe_fds, int i, int cmd_count)
{
	if (prev_pipe > 2)
		close(prev_pipe);
	if (pipe_fds[1] > 2)
		close(pipe_fds[1]);
	if (i == cmd_count - 1)
	{
		if (pipe_fds[0] > 2)
			close(pipe_fds[0]);
		return (STDIN_FILENO);
	}
	return (pipe_fds[0]);
}

/**
 * Setup pipes for the pipeline command.
 *
 * @param pipe_fds Array to store pipe file descriptors.
 * @param i Current command index.
 * @param cmd_count Total number of commands.
 * @param prev_pipe Previous pipe read end.
 * @return The updated previous pipe, or -1 on error.
 */
int	setup_and_validate_pipes(int *pipe_fds, int i, int cmd_count, int prev_pipe)
{
	if (handle_pipe_setup(pipe_fds, i, cmd_count) == -1)
	{
		perror("minishell: pipe setup failed");
		if (prev_pipe > 2)
			close(prev_pipe);
		return (-1);
	}
	return (prev_pipe);
}
