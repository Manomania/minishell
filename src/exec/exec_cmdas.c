/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmdas.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/30 16:37:25 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/31 12:55:19 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "debug.h"
#include "error.h"
#include "memory.h"
#include "minishell.h"

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
 * @brief Handles descriptor management after fork, avoiding double-close errors
 *
 * This function safely manages file descriptors during pipeline execution,
 * ensuring we don't close the same descriptor twice.
 *
 * @param prev_pipe Previous pipe's read end
 * @param pipe_fds Current pipe file descriptors
 * @param i Current command index
 * @param cmd_count Total command count
 * @return int Updated previous pipe file descriptor
 */
int	handle_descriptors(int *prev_pipe, int pipe_fds[2], int i, int cmd_count)
{
	if (*prev_pipe != STDIN_FILENO && *prev_pipe > 0)
	{
		close(*prev_pipe);
		*prev_pipe = -1;
	}
	if (pipe_fds[1] != STDOUT_FILENO && pipe_fds[1] > 0)
	{
		close(pipe_fds[1]);
		pipe_fds[1] = -1;
	}
	if (i < cmd_count - 1)
		return (pipe_fds[0]);
	return (STDIN_FILENO);
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
	pid_t	pid;
	int		result;

	if (handle_pipe_setup(data->pipe_fds, data->i, data->cmd_count) == -1)
		return (-1);
	setup_parent_signals();
	result = handle_non_builtin(ctx, data);
	if (result == -1)
		return (-1);
	else if (result > 0)
		return (result);
	pid = execute_pipeline_command(ctx, data->current, &data->prev_pipe,
			&data->pipe_fds[1]);
	data->pids[data->i] = pid;
	return (handle_descriptors(&data->prev_pipe, data->pipe_fds, data->i,
			data->cmd_count));
}

static t_bool	exec_all_cmdas(t_ctx *ctx, t_pipe_data data,
		t_command **cmd_head)
{
	int	i;

	i = 0;
	while (i < data.cmd_count)
	{
		ctx->cmd = data.current;
		data.prev_pipe = process_pipeline_cmd(ctx, &data);
		if (data.prev_pipe == -1)
		{
			ctx->cmd = *cmd_head;
			free(data.pids);
			ctx->exit_status = error(NULL, "exec_all_cmdas", ERR_PIPE);
			return (false);
		}
		data.current = data.current->next;
		data.i++;
		i++;
	}
	return (true);
}

/**
 * @brief Execute the commands in a pipeline
 *
 * @param ctx Context
 * @return Exit status of the last command
 */
int	exec_cmdas(t_ctx *ctx)
{
	t_pipe_data	data;
	int			exit_status;
	t_command	*cmd_head;

	if (!init_pipe_data(&data, ctx))
	{
		ctx->exit_status = error(NULL, "init_pipe_data", ERR_ALLOC);
		return (ctx->exit_status);
	}
	setup_parent_signals();
	cmd_head = ctx->cmd;
	if (!exec_all_cmdas(ctx, data, &cmd_head))
		return (ctx->exit_status);
	exit_status = wait_for_pipeline_processes(data.pids, data.cmd_count);
	setup_signals();
	free(data.pids);
	ctx->cmd = cmd_head;
	return (exit_status);
}
