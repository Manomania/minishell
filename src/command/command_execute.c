/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_execute.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 16:10:59 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/07 20:24:07 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "debug.h"
#include "error.h"
#include "minishell.h"

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
	int		was_signaled;

	was_signaled = 0;
	if (!command_bin(ctx))
	{
		if (ctx->cmd->args && ctx->cmd->args[0])
		{
			if (ctx->cmd->args[0][0] == '.' && !ft_strchr(ctx->cmd->args[0],
					'/'))
				return (error_code(ERR_NO_FILE));
			if (ft_strchr(ctx->cmd->args[0], '/'))
				return (error_code(ERR_IS_DIR));
			return (error(ctx->cmd->args[0], "exec", ERR_CMD_NOT_FOUND));
		}
		return (error_code(ERR_CMD_NOT_FOUND));
	}
	setup_parent_signals();
	pid = fork();
	if (pid == -1)
		return (error(NULL, "execute_single_command", ERR_CHILD));
	if (pid == 0)
		execute_child(ctx);
	waitpid(pid, &status, 0);
	if (WIFSIGNALED(status))
		was_signaled = 1;
	setup_signals();
	if (was_signaled && isatty(STDOUT_FILENO))
		write(STDOUT_FILENO, "\n", 1);
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
 * @brief Validates if the command context is valid for execution
 *
 * @param ctx Context containing environment and command info
 * @param status Pointer to store error status
 * @return int 1 if valid, 0 if invalid
 */
static int	validate_command_context(t_ctx *ctx, int *status)
{
	if (!ctx || !ctx->cmd)
	{
		*status = error(NULL, "validate_command_context", ERR_ALLOC);
		ctx->exit_status = *status;
		return (0);
	}
	if (!ctx->cmd->args && !ctx->cmd->redirection)
	{
		ctx->exit_status = 0;
		*status = 0;
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
static int	process_command(t_ctx *ctx)
{
	int	status;

	if (has_only_redirections(ctx->cmd))
		return (execute_redirections_only(ctx));
	if (is_pipeline(ctx->cmd))
		status = exec_cmdas(ctx);
	else
	{
		if (builtins_try(ctx, ctx->cmd))
			return (ctx->exit_status);
		status = execute_single_command(ctx);
	}
	return (status);
}

/**
 * @brief Executes a command or pipeline
 *
 * @param ctx Context containing environment and command info
 * @return int Exit status of the command
 */
int	command_execute(t_ctx *ctx)
{
	int	status;
	int	is_valid;

	status = 0;
	is_valid = validate_command_context(ctx, &status);
	if (!is_valid)
		return (status);
	status = process_command(ctx);
	ctx->exit_status = status;
	return (status);
}
