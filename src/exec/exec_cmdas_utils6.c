/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmdas_utils6.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 17:55:33 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/21 17:58:05 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"

/**
 * Process one command in the pipeline
 *
 * @param ctx Context information
 * @param data Pipeline data structure
 * @return Updated prev_pipe descriptor or -1 on error
 */
static int	process_pipeline_cmd(t_ctx *ctx, t_pipe_data *data)
{
	pid_t	pid;
	int		result;
	int		next_pipe;

	if (handle_pipe_setup(data->pipe_fds, data->i, data->cmd_count) == -1)
		return (-1);
	setup_parent_signals();
	result = handle_non_builtin(ctx, data);
	if (result == -1)
	{
		cleanup_pipe_fds(data->pipe_fds);
		return (-1);
	}
	else if (result > 0)
		return (result);
	pid = execute_pipeline_cmd_with_redir(ctx, data, data->prev_pipe,
			data->pipe_fds[1]);
	if (pid == -1)
	{
		cleanup_pipe_fds(data->pipe_fds);
		return (-1);
	}
	next_pipe = handle_descriptors(data->prev_pipe, data->pipe_fds, data->i,
			data->cmd_count);
	return (next_pipe);
}

/**
 * Handle error during pipeline execution
 *
 * @param pids Process IDs array
 * @param ctx Shell context
 * @return Error status
 */
static int	handle_pipeline_error(pid_t *pids, t_ctx *ctx)
{
	free(pids);
	ctx->exit_status = error(NULL, "exec_all_cmdas", ERR_PIPE);
	return (0);
}

/**
 * Execute commands in pipeline
 *
 * @param ctx Context information
 * @param data Pipeline data structure
 * @param cmd_head Pointer to store original command head
 * @return True if successful, false on error
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
			return (handle_pipeline_error(data.pids, ctx));
		}
		data.prev_pipe = pipe_result;
		cmd_iter = cmd_iter->next;
		data.current = cmd_iter;
		data.i++;
		i++;
	}
	return (1);
}

/**
 * Execute the commands in a pipeline
 *
 * @param ctx Context
 * @return Exit status of the last command
 */
int	exec_cmdas(t_ctx *ctx)
{
	t_pipe_data	data;
	int			exit_status;
	t_command	*cmd_head;
	t_bool		success;
	pid_t		*pids;

	if (!init_pipe_data(&data, ctx))
	{
		ctx->exit_status = error(NULL, "init_pipe_data", ERR_ALLOC);
		return (ctx->exit_status);
	}
	setup_parent_signals();
	cmd_head = ctx->cmd;
	pids = data.pids;
	success = exec_all_cmdas(ctx, data, &cmd_head);
	if (!success)
	{
		ctx->cmd = cmd_head;
		return (ctx->exit_status);
	}
	exit_status = wait_for_pipeline_processes(pids, data.cmd_count);
	setup_signals();
	free(pids);
	ctx->cmd = cmd_head;
	return (exit_status);
}
