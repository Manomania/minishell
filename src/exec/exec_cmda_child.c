/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmda_child.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/30 19:27:50 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/13 12:09:07 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Child cleanup - frees all resources in child process
 *
 * @param ctx Context to clean up
 * @param error_code Exit code to use
 */
static void	child_cleanup_exit(t_ctx *ctx, int error_code)
{
	ctx_clear(ctx);
	exit(error_code);
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
 * @brief Handles the execution of a command in a pipeline
 *
 * @param ctx Context
 * @param cmd Command to execute
 * @param pipes Array of pipe file descriptors
 * @param cmd_index Index of the command in the pipeline
 * @param cmd_count Total number of commands in the pipeline
 */
void	execute_command_in_pipeline(t_ctx *ctx, t_command *cmd, int pipes[2][2],
		int cmd_index, int cmd_count)
{
	char	**exec_args;

	if (pipes[1][0] >= 0)
		close(pipes[1][0]);
	if (cmd_index > 0 && pipes[0][0] >= 0)
	{
		dup2(pipes[0][0], STDIN_FILENO);
		close(pipes[0][0]);
	}
	else if (ctx->fd_file_in != -1)
	{
		dup2(ctx->fd_file_in, STDIN_FILENO);
		close(ctx->fd_file_in);
	}
	if (cmd_index < cmd_count - 1 && pipes[1][1] >= 0)
	{
		dup2(pipes[1][1], STDOUT_FILENO);
		close(pipes[1][1]);
	}
	else if (ctx->fd_file_out != -1)
	{
		dup2(ctx->fd_file_out, STDOUT_FILENO);
		close(ctx->fd_file_out);
	}
	if (pipes[0][1] >= 0)
		close(pipes[0][1]);
	if (handle_redirections(cmd->redirection) != 0)
		child_cleanup_exit(ctx, EXIT_FAILURE);
	if (builtins_try(ctx, cmd))
		child_cleanup_exit(ctx, EXIT_SUCCESS);
	if (!command_bin(ctx))
		child_cleanup_exit(ctx, EXIT_FAILURE);
	exec_args = prepare_execve_args(cmd);
	if (!exec_args)
		child_cleanup_exit(ctx, EXIT_FAILURE);
	execve(cmd->args[0], exec_args, ctx->envp);
	free_2d_array((void **)exec_args);
	perror("execve");
	child_cleanup_exit(ctx, EXIT_FAILURE);
}
