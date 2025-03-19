/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_execute_utils.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 17:54:30 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/19 18:22:55 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "debug.h"
#include "error.h"
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
		ft_printf("command not found or not executable: %s\n", cmd->args[0]);
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
void	execute_child(t_ctx *ctx)
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
int	get_exit_status(int status)
{
	int	exit_code;

	if (WIFEXITED(status))
	{
		exit_code = WEXITSTATUS(status);
		debug_log(DEBUG_VERBOSE, "exit_status", "Process exited normally");
		return (exit_code);
	}
	if (WIFSIGNALED(status))
	{
		exit_code = 128 + WTERMSIG(status);
		debug_log(DEBUG_VERBOSE, "exit_status", "Process terminated by signal");
		return (exit_code);
	}
	debug_log(DEBUG_VERBOSE, "exit_status", "Process ended in unknown state");
	return (1);
}
