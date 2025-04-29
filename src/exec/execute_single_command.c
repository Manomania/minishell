/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_single_command.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 12:15:06 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/29 18:32:13 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "error.h"
#include "minishell.h"

/**
 * @brief Executes a builtin command with proper redirection
 *
 * @param ctx Context containing environment and command info
 * @param saved_stdin File descriptor for saved stdin
 * @param saved_stdout File descriptor for saved stdout
 * @return Exit status of the builtin command
 */
int	execute_builtin_command(t_ctx *ctx, int saved_stdin, int saved_stdout)
{
	int	exit_code;
	int	result;

	exit_code = 0;
	result = setup_command_redirections(ctx, ctx->cmd);
	if (result != 0)
		exit_code = 1;
	else
	{
		builtins_try(ctx, ctx->cmd);
		exit_code = ctx->exit_status;
	}
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
	return (exit_code);
}

/**
 * @brief Handles parent process waiting and signal processing more robustly.
 *
 * @param pid Child process ID to wait for
 * @param saved_stdin Saved standard input descriptor (to close)
 * @param saved_stdout Saved standard output descriptor (to close)
 * @return Exit status of the command
 */
static int	handle_parent_process(pid_t pid, int saved_stdin, int saved_stdout)
{
	int	status;
	int	exit_code;

	if (saved_stdin != -1)
		close(saved_stdin);
	if (saved_stdout != -1)
		close(saved_stdout);
	exit_code = 1;
	if (waitpid(pid, &status, 0) == -1)
		perror("minishell: waitpid");
	else
	{
		if (WIFEXITED(status))
			exit_code = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
		{
			exit_code = 128 + WTERMSIG(status);
			if (WTERMSIG(status) == SIGQUIT)
				ft_printf_fd(STDERR_FILENO, "Quit (core dumped)\n");
			else if (WTERMSIG(status) == SIGINT && isatty(STDOUT_FILENO))
				write(STDOUT_FILENO, "\n", 1);
		}
	}
	setup_signals();
	return (exit_code);
}

/**
 * @brief Executes an external command with fork
 *
 * @param ctx Context containing environment and command info
 * @param saved_stdin Saved standard input descriptor
 * @param saved_stdout Saved standard output descriptor
 * @return Exit status of the external command
 */
static int	execute_external_command(t_ctx *ctx, int saved_stdin,
		int saved_stdout)
{
	pid_t	pid;
	int		exit_code;

	setup_parent_signals();
	pid = fork();
	if (pid == -1)
	{
		if (saved_stdin != -1)
			close(saved_stdin);
		if (saved_stdout != -1)
			close(saved_stdout);
		setup_signals();
		return (error(NULL, "fork", ERR_CHILD));
	}
	if (pid == 0)
		execute_child_process(ctx, saved_stdin, saved_stdout);
	exit_code = handle_parent_process(pid, saved_stdin, saved_stdout);
	return (exit_code);
}

/**
 * @brief Executes a single command without a pipeline
 *
 * @param ctx Context containing environment and command info
 * @return Exit status of the command
 */
int	execute_single_command(t_ctx *ctx)
{
	int		saved_stdin;
	int		saved_stdout;
	char	*arg;

	saved_stdin = dup(STDIN_FILENO);
	saved_stdout = dup(STDOUT_FILENO);
	if (saved_stdin == -1 || saved_stdout == -1)
	{
		if (saved_stdin != -1)
			close(saved_stdin);
		if (saved_stdout != -1)
			close(saved_stdout);
		return (error(NULL, "dup", ERR_FD));
	}
	arg = NULL;
	if (ctx->cmd && ctx->cmd->args && ctx->cmd->args[0])
		arg = ctx->cmd->args[0];
	if (is_builtin_command(arg))
		return (execute_builtin_command(ctx, saved_stdin, saved_stdout));
	else
		return (execute_external_command(ctx, saved_stdin, saved_stdout));
}
