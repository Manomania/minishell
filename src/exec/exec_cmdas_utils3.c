/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmdas_utils3.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 15:32:57 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/24 14:13:25 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "minishell.h"

/**
 * @brief Executes a command that only has redirections in a pipeline
 *
 * @param ctx Context containing environment information
 * @param data Pipeline data structure
 * @return pid_t Process ID or -1 on error
 */
pid_t	execute_redirections_only_pipeline(t_ctx *ctx, t_pipe_data *data)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
		return (-1);
	if (pid == 0)
	{
		reset_signals();
		setup_pipe_redirections(data);
		if (setup_heredocs(ctx, data->current) != 0
			|| setup_redirections(data->current->redirection) != 0)
		{
			cleanup_child_process(ctx);
			exit(EXIT_FAILURE);
		}
		cleanup_child_process(ctx);
		exit(EXIT_SUCCESS);
	}
	return (pid);
}

/**
 * Checks if command binary exists and is executable
 *
 * @param ctx Context information
 * @param data Structure with pipe information
 * @return t_bool true if binary found, false otherwise
 */
t_bool	check_command_binary(t_ctx *ctx, t_pipe_data *data)
{
	t_bool	bin_found;

	bin_found = command_bin(ctx);
	if (bin_found == false)
	{
		if (data->current->args && data->current->args[0])
		{
			free(data->current->args[0]);
			data->current->args[0] = NULL;
		}
		data->pids[data->i] = -1;
	}
	return (bin_found);
}

/**
 * Checks if the command has only redirections without an actual command
 *
 * @param command The command to check
 * @return t_bool true if command has only redirections, false otherwise
 */
t_bool	has_only_redirections_pipeline(t_command *command)
{
	return (command && command->redirection && (!command->args
			|| !command->args[0]));
}

/**
 * Validates if command is valid and setup for execution
 *
 * @param data Structure with pipe information
 * @return t_bool true if valid command, false otherwise
 */
t_bool	validate_pipeline_command(t_pipe_data *data)
{
	if (!data->current->args && !data->current->redirection)
		return (false);
	if (!data->current->args || !data->current->args[0])
	{
		if (data->current->redirection)
			return (true);
		return (false);
	}
	return (true);
}

/**
 * Handles non-builtin commands in pipeline
 *
 * @param ctx Context information
 * @param data Structure with pipe information
 * @return int 0 if command is valid to proceed, -1 if pipeline should stop,
 * prev_pipe value if command failed pre-fork but pipeline continues.
 */
int	handle_non_builtin(t_ctx *ctx, t_pipe_data *data)
{
	t_bool		bin_found;
	t_command	*original_cmd;

	if (!validate_pipeline_command(data))
	{
		data->pids[data->i] = 0;
		return (data->prev_pipe);
	}
	if (has_only_redirections_pipeline(data->current))
		return (0);
	if (!is_builtin_command(data->current->args[0]))
	{
		original_cmd = ctx->cmd;
		ctx->cmd = data->current;
		bin_found = command_bin(ctx);
		ctx->cmd = original_cmd;
		if (bin_found == false)
		{
			data->pids[data->i] = -1;
			return (data->prev_pipe);
		}
	}
	return (0);
}
