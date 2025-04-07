/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_execute_utils.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 17:54:30 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/07 20:01:34 by elagouch         ###   ########.fr       */
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
	if (!cmd->args || !cmd->args[0])
		return (0);
	if (access(cmd->args[0], X_OK) != 0)
	{
		ft_printf("command not found or not executable: %s\n", cmd->args[0]);
		return (1);
	}
	return (0);
}

/**
 * @brief Clean up all resources before child process exit
 *
 * @param ctx Context to clean up
 */
static void	child_process_cleanup(t_ctx *ctx)
{
	cleanup_heredoc_resources(ctx);
	if (ctx->tokens)
	{
		free_all_token(ctx->tokens);
		ctx->tokens = NULL;
	}
	ctx_clear(ctx);
}

t_bool	has_only_redirections(t_command *cmd)
{
	return (cmd && cmd->redirection && (!cmd->args || !cmd->args[0]));
}

/**
 * @brief Executes command or processes redirections if no command
 *
 * @param ctx Context with environment
 * @param cmd Command to execute
 */
static void	execute_command_or_redir(t_ctx *ctx, t_command *cmd)
{
	if (!cmd->args || !cmd->args[0])
	{
		child_process_cleanup(ctx);
		exit(EXIT_SUCCESS);
	}
	execve(cmd->args[0], cmd->args, ctx->envp);
	perror("execve");
	child_process_cleanup(ctx);
	exit(EXIT_FAILURE);
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
	{
		child_process_cleanup(ctx);
		exit(EXIT_FAILURE);
	}
	cmd_check = check_command_executable(ctx->cmd);
	if (cmd_check != 0)
	{
		child_process_cleanup(ctx);
		exit(EXIT_FAILURE);
	}
	execute_command_or_redir(ctx, ctx->cmd);
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
		return (exit_code);
	}
	if (WIFSIGNALED(status))
	{
		exit_code = 128 + WTERMSIG(status);
		return (exit_code);
	}
	return (1);
}
