/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmdas_utils6.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 17:55:33 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/24 16:10:50 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "error.h"
#include "minishell.h"

/**
 * Create and manage the child process
 *
 * @param ctx Context information
 * @param data Pipeline data structure
 * @param next_read Next pipe read end (read end of the *current* pipe)
 * @return -1 on error, 0 on success
 */
static int	create_child_process(t_ctx *ctx, t_pipe_data *data, int next_read)
{
	pid_t	pid;

	(void)next_read;
	pid = fork();
	if (pid == -1)
	{
		perror("minishell: fork");
		cleanup_pipe_fds(data->pipe_fds);
		if (data->prev_pipe > 2)
			close(data->prev_pipe);
		return (-1);
	}
	if (pid == 0)
	{
		free(data->pids);
		if (setup_child_redirections_bruh(ctx, data->current, data->prev_pipe,
				data->pipe_fds[1]) != 0)
		{
			cleanup_child_process(ctx);
			exit(EXIT_FAILURE);
		}
		handle_child_process(ctx, data->current, data->pipe_fds[0]);
	}
	data->pids[data->i] = pid;
	return (0);
}

/**
 * Handle parent process pipe descriptors
 *
 * @param prev_pipe Previous pipe descriptor
 * @param pipe_fds Current pipe file descriptors
 * @param i Current command index
 * @param cmd_count Total command count
 * @return Updated prev_pipe value
 */
static int	update_parent_pipes(int prev_pipe, int *pipe_fds, int i,
		int cmd_count)
{
	int	result;

	result = handle_descriptors(prev_pipe, pipe_fds, i, cmd_count);
	return (result);
}

/**
 * Process one command in the pipeline: setup pipes, fork, execute, manage FDs.
 *
 * @param ctx Context information
 * @param data Pipeline data structure
 * @return Updated prev_pipe descriptor for the next stage, or -1 on error.
 */
static int	process_pipeline_cmd(t_ctx *ctx, t_pipe_data *data)
{
	int	result;
	int	next_pipe_read_end;

	if (handle_pipe_setup(data->pipe_fds, data->i, data->cmd_count) == -1)
		return (-1);
	next_pipe_read_end = data->pipe_fds[0];
	setup_parent_signals();
	result = handle_non_builtin(ctx, data);
	if (result == -1)
	{
		cleanup_pipe_fds(data->pipe_fds);
		return (-1);
	}
	else if (result > 0)
	{
		cleanup_pipe_fds(data->pipe_fds);
		return (result);
	}
	if (create_child_process(ctx, data, next_pipe_read_end) == -1)
		return (-1);
	data->prev_pipe = update_parent_pipes(data->prev_pipe, data->pipe_fds,
			data->i, data->cmd_count);
	return (data->prev_pipe);
}

/**
 * Execute all commands in a pipeline sequence.
 *
 * @param ctx Context information
 * @param data Pipeline data structure
 * @param cmd_head Pointer to store original command head (for context reset)
 * @return True if pipeline setup was successful, false on critical error.
 */
static t_bool	exec_all_cmdas(t_ctx *ctx, t_pipe_data data,
		t_command **cmd_head)
{
	int			i;
	int			pipe_result;
	t_command	*cmd_iter;

	i = 0;
	cmd_iter = data.current;
	while (i < data.cmd_count)
	{
		ctx->cmd = cmd_iter;
		pipe_result = process_pipeline_cmd(ctx, &data);
		if (pipe_result == -1)
		{
			ctx->cmd = *cmd_head;
			return (false);
		}
		cmd_iter = cmd_iter->next;
		data.current = cmd_iter;
		data.i++;
		i++;
	}
	ctx->cmd = *cmd_head;
	return (true);
}

/**
 * Execute the commands in a pipeline.
 * Manages creation of pipes, forking processes, waiting for completion,
 * and returning the exit status of the last command.
 *
 * @param ctx Context
 * @return Exit status of the last command in the pipeline.
 */
int	exec_cmdas(t_ctx *ctx)
{
	t_pipe_data	data;
	int			exit_status;
	t_command	*cmd_head;
	t_bool		success;
	pid_t		*pids_copy;

	if (!init_pipe_data(&data, ctx))
	{
		ctx->exit_status = error(NULL, "init_pipe_data", ERR_ALLOC);
		return (ctx->exit_status);
	}
	cmd_head = ctx->cmd;
	pids_copy = data.pids;
	if (prepare_all_pipeline_files(cmd_head) != 0)
	{
		free(pids_copy);
		return (1);
	}
	success = exec_all_cmdas(ctx, data, &cmd_head);
	exit_status = wait_for_pipeline_processes(pids_copy, data.cmd_count, ctx);
	setup_signals();
	free(pids_copy);
	if (!success)
		if (exit_status == 0)
			exit_status = 1;
	return (exit_status);
}
