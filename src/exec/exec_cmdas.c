/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmdas.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/30 16:37:25 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/14 15:42:09 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Counts the number of commands in a pipeline
 *
 * @param cmd The first command in the pipeline
 * @return int The number of commands
 */
int	count_commands(t_command *cmd)
{
	int			count;
	t_command	*current;

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
 * @brief Handles pipe setup for commands
 *
 * @param pipe_fds Pipe file descriptors
 * @param i Index of current command
 * @param cmd_count Total command count
 * @return int 0 on success, -1 on error
 */
static int	handle_pipe_setup(int pipe_fds[2], int i, int cmd_count)
{
	if (i < cmd_count - 1)
	{
		if (setup_pipe(pipe_fds) == -1)
			return (-1);
	}
	else
		pipe_fds[1] = STDOUT_FILENO;
	return (0);
}

/**
 * @brief Handles descriptor management after fork
 *
 * @param prev_pipe Previous pipe's read end
 * @param pipe_fds Current pipe file descriptors
 * @param i Current command index
 * @param cmd_count Total command count
 * @return int Updated previous pipe file descriptor
 */
static int	handle_descriptors(int prev_pipe, int pipe_fds[2], int i,
		int cmd_count)
{
	if (prev_pipe != STDIN_FILENO)
		close(prev_pipe);
	if (pipe_fds[1] != STDOUT_FILENO)
		close(pipe_fds[1]);
	if (i < cmd_count - 1)
		return (pipe_fds[0]);
	return (prev_pipe);
}

/**
 * @brief Execute one command in the pipeline process
 *
 * @param ctx Context information
 * @param data Structure with pipe information
 * @return int Updated previous pipe file descriptor
 */
static int	process_pipeline_cmd(t_ctx *ctx, t_pipe_data *data)
{
	if (handle_pipe_setup(data->pipe_fds, data->i, data->cmd_count) == -1)
		return (-1);
	setup_parent_signals();
	data->pids[data->i] = exec_piped_command(ctx, data->current,
			data->prev_pipe, data->pipe_fds[1]);
	return (handle_descriptors(data->prev_pipe, data->pipe_fds, data->i,
			data->cmd_count));
}

/**
 * @brief Initialize pipe data structure
 *
 * @param data Pipe data structure to initialize
 * @param ctx Context with command info
 * @return t_bool TRUE on success, FALSE on failure
 */
static t_bool	init_pipe_data(t_pipe_data *data, t_ctx *ctx)
{
	data->current = ctx->cmd;
	data->cmd_count = count_commands(data->current);
	data->i = 0;
	data->prev_pipe = STDIN_FILENO;
	data->pids = malloc(sizeof(pid_t) * (size_t)data->cmd_count);
	if (!data->pids)
		return (false);
	return (true);
}

/**
 * @brief Execute the commands in a pipeline
 *
 * @param ctx Context
 * @return int Exit status of the last command
 */
t_bool	exec_cmdas(t_ctx *ctx)
{
	t_pipe_data	data;
	int			exit_status;

	if (!init_pipe_data(&data, ctx))
		return (ctx_error(ERR_ALLOC));
	setup_parent_signals();
	while (data.i < data.cmd_count)
	{
		data.prev_pipe = process_pipeline_cmd(ctx, &data);
		if (data.prev_pipe == -1)
		{
			free(data.pids);
			return (ctx_error(ERR_PIPE));
		}
		data.current = data.current->next;
		data.i++;
	}
	exit_status = wait_for_pids(data.pids, data.cmd_count);
	setup_signals();
	free(data.pids);
	return (exit_status);
}
