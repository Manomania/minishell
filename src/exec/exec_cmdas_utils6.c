/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmdas_utils6.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 17:55:33 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/28 13:28:08 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "error.h"
#include "minishell.h"

/**
 * Create and manage the child process for a pipeline stage.
 * Handles forking, setting up child redirections/pipes, and execution.
 *
 * @param ctx Context information.
 * @param data Pipeline data structure containing current command, pids array,
 * etc.
 * @param next_read_end Read end of the pipe created for the *next* command
 * stage.
 * The current child needs to close this.
 * @return Process ID of the created child, or -1 on fork error.
 */
static pid_t	create_child_process(t_ctx *ctx, t_pipe_data *data,
		int next_read_end)
{
	pid_t	pid;

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
		handle_child_process(ctx, data->current, next_read_end);
		cleanup_child_process(ctx);
		exit(EXIT_FAILURE);
	}
	return (pid);
}

/**
 * Handle validation and cleanup for non-builtin commands.
 *
 * @param ctx Context information.
 * @param data Pipeline data structure.
 * @param pipe_fds Array of pipe file descriptors.
 * @return 0 on success, >0 for command not found, -1 on error.
 */
static int	validate_and_cleanup(t_ctx *ctx, t_pipe_data *data, int *pipe_fds)
{
	int	validation_result;

	validation_result = handle_non_builtin(ctx, data);
	if (validation_result == -1)
	{
		cleanup_pipe_fds(pipe_fds);
		if (data->prev_pipe > 2)
			close(data->prev_pipe);
		return (-1);
	}
	else if (validation_result > 0)
	{
		cleanup_pipe_fds(pipe_fds);
		if (data->prev_pipe > 2)
			close(data->prev_pipe);
		data->pids[data->i] = -1;
		return (validation_result);
	}
	return (0);
}

/**
 * Process one command stage in the pipeline.
 * Sets up pipes, validates the command, forks, executes, and manages FDs.
 *
 * @param ctx Context information.
 * @param data Pipeline data structure (passed by pointer, modifications
 * persist).
 * @return The read end fd for the next stage, or -1 on critical error.
 */
static int	process_pipeline_cmd(t_ctx *ctx, t_pipe_data *data)
{
	int		validation_result;
	pid_t	child_pid;
	int		pipe_check;

	pipe_check = setup_and_validate_pipes(data->pipe_fds, data->i,
			data->cmd_count, data->prev_pipe);
	if (pipe_check == -1)
		return (-1);
	setup_parent_signals();
	validation_result = validate_and_cleanup(ctx, data, data->pipe_fds);
	if (validation_result != 0)
		return (-1);
	child_pid = create_child_process(ctx, data, data->pipe_fds[0]);
	if (child_pid == -1)
		return (-1);
	data->pids[data->i] = child_pid;
	data->prev_pipe = update_parent_pipes(data->prev_pipe, data->pipe_fds,
			data->i, data->cmd_count);
	return (data->prev_pipe);
}

/**
 * Execute all commands in a pipeline sequence by iterating through them.
 *
 * @param ctx Context information.
 * @param data Pipeline data structure (passed by value, careful with pointers
 * inside).
 * @param cmd_head Pointer to the original head of the command list (for context
 * reset).
 * @return True if all stages were initiated successfully, false on critical
 * error.
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
	if (!success && exit_status == 0)
		exit_status = 1;
	return (exit_status);
}
