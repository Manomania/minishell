/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_execute_utils6.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 16:26:01 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/21 18:03:54 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"

/**
 * @brief Handles parent process waiting and signal processing
 *
 * @param pid Child process ID to wait for
 * @param saved_stdin File descriptor for saved stdin
 * @param saved_stdout File descriptor for saved stdout
 * @return Exit status of the command
 */
int	handle_parent_process(pid_t pid, int saved_stdin, int saved_stdout)
{
	int	status;
	int	exit_code;

	close_standard_io(saved_stdin, saved_stdout);
	waitpid(pid, &status, 0);
	exit_code = get_exit_status(status);
	if (WIFSIGNALED(status))
		exit_code = process_signal_exit(status);
	setup_signals();
	return (exit_code);
}

/**
 * @brief Processes signal-related exit status
 *
 * @param status The status returned from waitpid
 * @return The calculated exit code
 */
int	process_signal_exit(int status)
{
	int	exit_code;

	exit_code = 128 + WTERMSIG(status);
	if (WTERMSIG(status) == SIGQUIT)
		ft_printf_fd(STDERR_FILENO, "Quit (core dumped)\n");
	else if (WTERMSIG(status) == SIGINT && isatty(STDOUT_FILENO))
		write(STDOUT_FILENO, "\n", 1);
	else if (WTERMSIG(status) == SIGPIPE)
		ft_printf_fd(STDERR_FILENO, "Broken pipe: %d\n", WTERMSIG(status));
	else if (WTERMSIG(status) != SIGINT && isatty(STDOUT_FILENO))
		write(STDOUT_FILENO, "\n", 1);
	return (exit_code);
}

/**
 * @brief Handles the execution of external commands
 *
 * @param ctx Context containing environment and command info
 * @param saved_stdin File descriptor for saved stdin
 * @param saved_stdout File descriptor for saved stdout
 * @return Exit status of the external command
 */
int	execute_external_command(t_ctx *ctx, int saved_stdin, int saved_stdout)
{
	pid_t	pid;
	int		exit_code;

	setup_parent_signals();
	pid = fork();
	if (pid == -1)
	{
		handle_standard_io(saved_stdin, saved_stdout);
		setup_signals();
		return (error(NULL, "fork", ERR_CHILD));
	}
	if (pid == 0)
	{
		execute_child_process(ctx, saved_stdin, saved_stdout);
		exit_code = ctx->exit_status;
	}
	else
	{
		exit_code = handle_parent_process(pid, saved_stdin, saved_stdout);
	}
	return (exit_code);
}

/**
 * @brief Checks if the command is part of a pipeline
 *
 * @param cmd Command to check
 * @return true if command is part of pipeline, false otherwise
 */
static t_bool	is_pipeline(t_command *cmd)
{
	return (cmd && cmd->next);
}

/**
 * @brief Determines command type and executes appropriate handler
 *
 * @param ctx Context containing environment and command info
 * @return int Status code from command execution
 */
int	process_command_type(t_ctx *ctx)
{
	if (has_only_redirections(ctx->cmd))
		return (execute_single_command(ctx));
	else if (is_pipeline(ctx->cmd))
		return (exec_cmdas(ctx));
	else
		return (execute_single_command(ctx));
}
