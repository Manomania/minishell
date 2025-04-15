/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_execute.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 16:10:59 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/15 16:13:55 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "debug.h"
#include "error.h"
#include "minishell.h"

/**
 * @brief Sets up redirections for a command, stopping on the first error.
 * Also handles pre-reading heredocs if not already done.
 *
 * @param ctx Context containing environment info
 * @param cmd Command containing redirections to set up
 * @return int 0 on success, 1 on error (generic redirection failure)
 */
static int	setup_command_redirections(t_ctx *ctx, t_command *cmd)
{
	if (setup_heredocs(ctx, cmd) != 0)
		return (1);
	if (setup_redirections(cmd->redirection) != 0)
		return (1);
	return (0);
}

/**
 * @brief Handles command validation and error reporting in child process
 *
 * @param ctx Context containing environment info
 * @return int Exit code for the child process (0 on success, error code
 * otherwise)
 */
static int	validate_cmd_in_child(t_ctx *ctx)
{
	if (!ctx->cmd->args || !ctx->cmd->args[0])
		return (EXIT_SUCCESS);
	if (!command_bin(ctx))
	{
		if (ctx->cmd->args && ctx->cmd->args[0])
		{
			if (ft_strchr(ctx->cmd->args[0], '/'))
				return (126);
			else
				return (127);
		}
		return (127);
	}
	return (0);
}

/**
 * @brief Executes a single command without a pipeline
 *
 * @param ctx Context containing environment and command info
 * @return Exit status of the command
 */
static int	execute_single_command(t_ctx *ctx)
{
	pid_t	pid;
	int		status;
	int		was_signaled;
	int		saved_stdout;
	int		exit_code;
	int		saved_stdin;

	saved_stdin = -1;
	saved_stdout = -1;
	was_signaled = 0;
	exit_code = 0;
	saved_stdin = dup(STDIN_FILENO);
	saved_stdout = dup(STDOUT_FILENO);
	if (is_builtin_command(ctx->cmd->args ? ctx->cmd->args[0] : NULL))
	{
		if (setup_command_redirections(ctx, ctx->cmd) != 0)
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
	setup_parent_signals();
	pid = fork();
	if (pid == -1)
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
		setup_signals();
		return (error(NULL, "fork", ERR_CHILD));
	}
	if (pid == 0)
	{
		if (saved_stdin != -1)
			close(saved_stdin);
		if (saved_stdout != -1)
			close(saved_stdout);
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
	else
	{
		if (saved_stdin != -1)
			close(saved_stdin);
		if (saved_stdout != -1)
			close(saved_stdout);
		waitpid(pid, &status, 0);
		exit_code = get_exit_status(status);
		if (WIFSIGNALED(status))
		{
			was_signaled = 1;
			if (WTERMSIG(status) == SIGQUIT)
				ft_putstr_fd("Quit (core dumped)\n", STDERR_FILENO);
			else if (WTERMSIG(status) == SIGINT && isatty(STDOUT_FILENO))
				write(STDOUT_FILENO, "\n", 1);
			else if (WTERMSIG(status) == SIGPIPE)
			{
				ft_putstr_fd("Broken pipe: ", STDERR_FILENO);
				ft_putnbr_fd(WTERMSIG(status), STDERR_FILENO);
				ft_putstr_fd("\n", STDERR_FILENO);
			}
			else if (WTERMSIG(status) != SIGINT && isatty(STDOUT_FILENO))
				write(STDOUT_FILENO, "\n", 1);
			exit_code = 128 + WTERMSIG(status);
		}
		setup_signals();
		return (exit_code);
	}
	return (ctx->exit_status);
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
 * @brief Validates if the command context is valid for execution
 *
 * @param ctx Context containing environment and command info
 * @param status Pointer to store error status (unused here, handled by return)
 * @return int 1 if valid, 0 if invalid or nothing to execute
 */
static int	validate_command_context(t_ctx *ctx)
{
	if (!ctx || !ctx->cmd)
	{
		error(NULL, "validate_command_context", ERR_ALLOC);
		ctx->exit_status = 1;
		return (0);
	}
	if (!ctx->cmd->args && !ctx->cmd->redirection)
	{
		ctx->exit_status = 0;
		return (0);
	}
	return (1);
}

/**
 * @brief Determines command type and executes appropriate handler
 *
 * @param ctx Context containing environment and command info
 * @return int Status code from command execution
 */
static int	process_command_type(t_ctx *ctx)
{
	if (has_only_redirections(ctx->cmd))
		return (execute_single_command(ctx));
	else if (is_pipeline(ctx->cmd))
		return (exec_cmdas(ctx));
	else
		return (execute_single_command(ctx));
}

/**
 * @brief Main execution entry point for a parsed command line.
 * Handles heredocs, validates context, and dispatches execution.
 *
 * @param ctx Context containing environment and command info
 * @return int Exit status of the command/pipeline
 */
int	command_execute(t_ctx *ctx)
{
	int	status;

	if (!validate_command_context(ctx))
		return (ctx->exit_status);
	if (read_all_heredocs(ctx) != 0)
	{
		close_heredoc_fds(ctx->cmd);
		ctx->exit_status = 1;
		return (ctx->exit_status);
	}
	status = process_command_type(ctx);
	close_heredoc_fds(ctx->cmd);
	ctx->exit_status = status;
	return (status);
}
