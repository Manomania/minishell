/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmdas_utils3.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 15:32:57 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/31 14:38:37 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "minishell.h"

/**
 * @brief Checks if command binary exists and is executable
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
		if (data->current->args[0])
		{
			free(data->current->args[0]);
			data->current->args[0] = NULL;
		}
		data->pids[data->i] = -1;
		if (data->i == data->cmd_count - 1)
			ctx->exit_status = 127;
	}
	return (bin_found);
}

/**
 * @brief Validates if command is valid and setup for execution
 *
 * @param data Structure with pipe information
 * @return t_bool true if valid command, false otherwise
 */
t_bool	validate_pipeline_command(t_pipe_data *data)
{
	if (!data->current->args || !data->current->args[0])
	{
		data->pids[data->i] = -1;
		return (false);
	}
	return (true);
}

/**
 * @brief Handles non-builtin commands in pipeline
 *
 * @param ctx Context information
 * @param data Structure with pipe information
 * @return int -1 on error, updated previous pipe fd otherwise
 */
int	handle_non_builtin(t_ctx *ctx, t_pipe_data *data)
{
	t_bool	bin_found;

	if (!validate_pipeline_command(data))
	{
		return (handle_descriptors(&data->prev_pipe, data->pipe_fds, data->i,
				data->cmd_count));
	}
	if (!is_builtin_command(data->current->args[0]))
	{
		bin_found = check_command_binary(ctx, data);
		if (bin_found == false)
		{
			return (handle_descriptors(&data->prev_pipe, data->pipe_fds,
					data->i, data->cmd_count));
		}
	}
	return (0);
}
