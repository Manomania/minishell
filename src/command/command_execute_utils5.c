/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_execute_utils5.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 16:37:14 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/21 16:58:50 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "error.h"
#include "minishell.h"

/**
 * @brief Handles standard input/output file descriptor duplication and closing
 *
 * @param saved_stdin File descriptor for saved stdin
 * @param saved_stdout File descriptor for saved stdout
 */
void	handle_standard_io(int saved_stdin, int saved_stdout)
{
	if (saved_stdin != -1)
	{
		dup2(saved_stdin, STDIN_FILENO);
		close(saved_stdin);
	}
	if (saved_stdout != -1)
	{
		dup2(saved_stdout, STDOUT_FILENO);
		close(saved_stdout);
	}
}

/**
 * @brief Closes standard input/output file descriptors
 *
 * @param saved_stdin File descriptor for saved stdin
 * @param saved_stdout File descriptor for saved stdout
 */
void	close_standard_io(int saved_stdin, int saved_stdout)
{
	if (saved_stdin != -1)
		close(saved_stdin);
	if (saved_stdout != -1)
		close(saved_stdout);
}

/**
 * @brief Executes a builtin command with proper redirection
 *
 * @param ctx Context containing environment and command info
 * @param saved_stdin File descriptor for saved stdin
 * @param saved_stdout File descriptor for saved stdout
 * @return Exit status of the builtin command
 */
static int	execute_builtin_internal(t_ctx *ctx, int saved_stdin,
		int saved_stdout)
{
	int	exit_code;

	exit_code = 0;
	if (setup_command_redirections(ctx, ctx->cmd) != 0)
		exit_code = 1;
	else
	{
		builtins_try(ctx, ctx->cmd);
		exit_code = ctx->exit_status;
	}
	handle_standard_io(saved_stdin, saved_stdout);
	return (exit_code);
}

/**
 * @brief Handles child process execution
 *
 * @param ctx Context containing environment and command info
 * @param saved_stdin File descriptor for saved stdin
 * @param saved_stdout File descriptor for saved stdout
 */
void	execute_child_process(t_ctx *ctx, int saved_stdin, int saved_stdout)
{
	int	exit_code;

	close_standard_io(saved_stdin, saved_stdout);
	reset_signals();
	if (setup_command_redirections(ctx, ctx->cmd) != 0)
	{
		cleanup_child_process(ctx);
		exit(1);
	}
	exit_code = validate_cmd_in_child(ctx);
	if (exit_code != 0)
	{
		cleanup_child_process(ctx);
		exit(exit_code);
	}
	if (ctx->cmd->args && ctx->cmd->args[0])
	{
		execve(ctx->cmd->args[0], ctx->cmd->args, ctx->envp);
		ctx_error_exit(ctx, ctx->cmd->args[0], "execve", ERR_CHILD);
	}
	else
	{
		cleanup_child_process(ctx);
		exit(0);
	}
}

/**
 * @brief Executes a single command without a pipeline
 *
 * @param ctx Context containing environment and command info
 * @return Exit status of the command
 */
int	execute_single_command(t_ctx *ctx)
{
	int		saved_stdout;
	int		saved_stdin;
	char	*arg;

	saved_stdin = dup(STDIN_FILENO);
	saved_stdout = dup(STDOUT_FILENO);
	arg = NULL;
	if (ctx->cmd->args)
		arg = ctx->cmd->args[0];
	if (is_builtin_command(arg))
		return (execute_builtin_internal(ctx, saved_stdin, saved_stdout));
	return (execute_external_command(ctx, saved_stdin, saved_stdout));
}
