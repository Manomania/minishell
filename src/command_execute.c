/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_execute.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 16:10:59 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/08 18:37:56 by elagouch         ###   ########.fr       */
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
	reset_signals();
	if (cmd_index > 0)
		dup2(pipes[0][0], STDIN_FILENO);
	if (cmd_index < cmd_count - 1)
		dup2(pipes[1][1], STDOUT_FILENO);
	close_all_pipes(pipes);
	if (builtins_try(ctx, cmd))
		child_cleanup_exit(ctx, EXIT_SUCCESS);
	if (command_execute(ctx, cmd) != EXIT_SUCCESS)
		child_cleanup_exit(ctx, EXIT_FAILURE);
	child_cleanup_exit(ctx, EXIT_SUCCESS);
}

/**
 * @brief Ensure command has arguments array properly initialized
 *
 * @param cmd Command to initialize arguments for
 * @return int 0 on success, -1 on error
 */
static int	ensure_arguments(t_command *cmd)
{
	if (!cmd->cmd)
		return (-1);
	if (!cmd->args)
	{
		cmd->args = malloc(sizeof(char *) * 2);
		if (!cmd->args)
			return (-1);
		cmd->args[0] = ft_strdup(cmd->cmd);
		if (!cmd->args[0])
		{
			free(cmd->args);
			cmd->args = NULL;
			return (-1);
		}
		cmd->args[1] = NULL;
		cmd->arg_count = 1;
	}
	else if (cmd->args[0] == NULL)
	{
		cmd->args[0] = ft_strdup(cmd->cmd);
		if (!cmd->args[0])
			return (-1);
	}
	return (0);
}

/**
 * @brief Executes a command with execve
 *
 * @param ctx Context containing environment
 * @param cmd Command to execute
 * @return int 0 on success, -1 on error
 */
int	command_execute(t_ctx *ctx, t_command *cmd)
{
	char	*bin_path;

	if (!cmd->cmd)
		return (-1);
	if (builtins_try(ctx, cmd))
		return (0);
	bin_path = bin_find(ctx, cmd->cmd);
	if (!bin_path)
		return (ctx_error(ERR_CMD_NOT_FOUND));
	if (ensure_arguments(cmd) != 0)
	{
		free(bin_path);
		return (ctx_error(ERR_ALLOC));
	}
	if (handle_redirections(cmd->redirections) != 0)
	{
		free(bin_path);
		return (-1);
	}
	execve(bin_path, cmd->args, ctx->envp);
	free(bin_path);
	return (ctx_error(ERR_CMD_NOT_FOUND));
}
