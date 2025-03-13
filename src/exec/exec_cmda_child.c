/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmda_child.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/30 19:27:50 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/13 11:50:57 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Handles the input part of exec_cmda_child
 *
 * @param ctx Context
 * @param pipe_prev Previous pipe
 */
static void	handle_input(t_ctx *ctx, int *pipe_prev)
{
	if (pipe_prev[0] != -1)
	{
		dup2(pipe_prev[0], STDIN_FILENO);
		close(pipe_prev[0]);
		close(pipe_prev[1]);
	}
	else if (ctx->fd_file_in != -1)
	{
		dup2(ctx->fd_file_in, STDIN_FILENO);
		close(ctx->fd_file_in);
	}
}

/**
 * @brief Handles the output part of exec_cmda_child
 *
 * @param ctx Context
 * @param current_cmd Current command
 * @param pipe_curr Current pipe
 */
static void	handle_output(t_ctx *ctx, t_command *current_cmd, int *pipe_curr)
{
	if (current_cmd->next)
	{
		close(pipe_curr[0]);
		dup2(pipe_curr[1], STDOUT_FILENO);
		close(pipe_curr[1]);
	}
	else if (ctx->fd_file_out != -1)
	{
		dup2(ctx->fd_file_out, STDOUT_FILENO);
		close(ctx->fd_file_out);
	}
}

/**
 * @brief Prepares an array of arguments for execve
 *
 * @param cmd Command containing arguments
 * @return New array for execve or NULL on failure
 */
static char	**prepare_execve_args(t_command *cmd)
{
	char	**args;
	int		i;

	args = malloc(sizeof(char *) * (cmd->arg_count + 2));
	if (!args)
		return (NULL);
	i = 0;
	args[0] = ft_strdup(cmd->args[0]);
	if (!args[0])
	{
		free(args);
		return (NULL);
	}
	i = 0;
	while (i < cmd->arg_count)
	{
		args[i + 1] = ft_strdup(cmd->args[i + 1]);
		if (!args[i + 1])
		{
			while (i >= 0)
				free(args[i--]);
			free(args);
			return (NULL);
		}
		i++;
	}
	args[cmd->arg_count + 1] = NULL;
	return (args);
}

/**
 * @brief Executes a command with arguments: child POV
 *
 * @param ctx Context
 * @param current_cmd Current command
 * @param pipe_prev Previous pipe
 * @param pipe_curr Current pipe
 */
void	exec_cmda_child(t_ctx *ctx, t_command *current_cmd, int *pipe_prev,
		int *pipe_curr)
{
	char **exec_args;

	handle_input(ctx, pipe_prev);
	handle_output(ctx, current_cmd, pipe_curr);
	exec_args = prepare_execve_args(current_cmd);
	if (!exec_args)
	{
		ctx_clear(ctx);
		exit(EXIT_FAILURE);
	}
	execve(current_cmd->args[0], exec_args, ctx->envp);
	free_2d_array((void **)exec_args);
	ctx_clear(ctx);
	exit(EXIT_FAILURE);
}