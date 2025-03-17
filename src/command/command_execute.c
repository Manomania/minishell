/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_execute.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 16:10:59 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/17 14:35:30 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Sets up child process redirections
 *
 * This function handles heredocs and redirections in the child process.
 *
 * @param ctx Context with environment information
 * @param cmd Command containing redirections
 * @return 0 on success, non-zero on error
 */
static int	setup_child_redirections(t_ctx *ctx, t_command *cmd)
{
	int	result;

	result = setup_heredocs(ctx, cmd);
	if (result != 0)
		return (result);
	result = setup_redirections(cmd->redirection);
	return (result);
}

/**
 * @brief Checks if command exists and is executable
 *
 * This function verifies the command path and permissions.
 *
 * @param cmd Command to check
 * @return 0 on success, non-zero on error
 */
static int	check_command_executable(t_command *cmd)
{
	if (!cmd->args[0] || access(cmd->args[0], X_OK) != 0)
	{
		ft_printf("Command not found or not executable: %s\n", cmd->args[0]);
		return (1);
	}
	return (0);
}

/**
 * @brief Executes the child process portion of a single command
 *
 * This function handles the execution of a command in a child process.
 *
 * @param ctx Context containing environment and command info
 */
static void	execute_child(t_ctx *ctx)
{
	int	redirect_result;
	int	cmd_check;

	reset_signals();
	redirect_result = setup_child_redirections(ctx, ctx->cmd);
	if (redirect_result != 0)
		exit(EXIT_FAILURE);
	cmd_check = check_command_executable(ctx->cmd);
	if (cmd_check != 0)
		exit(EXIT_FAILURE);
	execve(ctx->cmd->args[0], ctx->cmd->args, ctx->envp);
	perror("execve");
	exit(EXIT_FAILURE);
}

/**
 * @brief Gets the exit status from process termination information
 *
 * This function extracts the exit status from the process status.
 *
 * @param status Process status from waitpid
 * @return Exit status of the process
 */
static int	get_exit_status(int status)
{
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}

/**
 * @brief Executes a single command without a pipeline
 *
 * This function executes a single command by forking a child process.
 *
 * @param ctx Context containing environment and command info
 * @return Exit status of the command
 */
static int	execute_single_command(t_ctx *ctx)
{
	pid_t	pid;
	int		status;

	if (builtins_try(ctx, ctx->cmd))
		return (0);
	if (!command_bin(ctx))
		return (ctx_error(ERR_CMD_NOT_FOUND));
	setup_parent_signals();
	pid = fork();
	if (pid == -1)
		return (ctx_error(ERR_CHILD));
	if (pid == 0)
		execute_child(ctx);
	waitpid(pid, &status, 0);
	setup_signals();
	return (get_exit_status(status));
}

/**
 * @brief Checks if the command is part of a pipeline
 *
 * This function determines if a command is part of a pipeline.
 *
 * @param cmd Command to check
 * @return true if command is part of pipeline, false otherwise
 */
static t_bool	is_pipeline(t_command *cmd)
{
	return (cmd && cmd->next);
}

/**
 * @brief Executes a command or pipeline
 *
 * This function executes a command or pipeline, handling redirections.
 *
 * @param ctx Context containing environment and command info
 * @return Exit status of the command
 */
int	command_execute(t_ctx *ctx)
{
	if (!ctx || !ctx->cmd)
		return (ctx_error(ERR_ALLOC));
	if (!ctx->cmd->args || !ctx->cmd->args[0])
		return (-1);
	if (is_pipeline(ctx->cmd))
		return (exec_cmdas(ctx));
	return (execute_single_command(ctx));
}
