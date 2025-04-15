/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_try.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 12:19:14 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/15 10:54:26 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "debug.h"
#include "error.h"
#include "minishell.h"

/**
 * @brief Executes a built-in command with proper redirection handling
 *
 * @param ctx Context for shell environment
 * @param cmd Command to execute
 * @param exit_status Pointer to store exit status
 * @return t_bool true if a built-in was executed, false otherwise
 */
static t_bool	execute_builtin(t_ctx *ctx, t_command *cmd, int *exit_status)
{
	int	saved_fds[2];
	int	redir_status;

	saved_fds[0] = dup(STDIN_FILENO);
	saved_fds[1] = dup(STDOUT_FILENO);
	if (saved_fds[0] == -1 || saved_fds[1] == -1)
	{
		if (saved_fds[0] != -1)
			close(saved_fds[0]);
		if (saved_fds[1] != -1)
			close(saved_fds[1]);
		(void)error(NULL, "execute_builtin", ERR_IO);
		return (false);
	}
	redir_status = setup_builtin_redirections(cmd, saved_fds);
	if (redir_status == -1)
	{
		*exit_status = 1;
		builtin_restore_redirections(saved_fds);
		return (true);
	}
	run_builtin_command(ctx, cmd, exit_status);
	builtin_restore_redirections(saved_fds);
	return (true);
}

/**
 * @brief Checks if the command is a recognized builtin
 *
 * @param cmd_name Name of the command to check
 * @return t_bool true if command is a builtin, false otherwise
 */
static t_bool	is_recognized_builtin(char *cmd_name)
{
	if (is_builtin(cmd_name, (char *)"exit") == 0 || is_builtin(cmd_name,
			(char *)"echo") == 0 || is_builtin(cmd_name, (char *)"cd") == 0
		|| is_builtin(cmd_name, (char *)"pwd") == 0 || is_builtin(cmd_name,
			(char *)"export") == 0 || is_builtin(cmd_name, (char *)"unset") == 0
		|| is_builtin(cmd_name, (char *)"env") == 0)
		return (true);
	return (false);
}

/**
 * @brief Validates the command arguments
 *
 * @param ctx Context for shell environment
 * @param cmd Command to validate
 * @return t_bool true if valid, false otherwise
 */
static t_bool	validate_command(t_ctx *ctx, t_command *cmd)
{
	if (!ctx || !cmd || !cmd->args || !cmd->args[0])
		return (false);
	return (true);
}

/**
 * @brief Tries to execute a builtin command
 *
 * @param ctx Context for shell environment
 * @param cmd Command to check for builtin
 * @return t_bool true if builtin was executed, false otherwise
 */
t_bool	builtins_try(t_ctx *ctx, t_command *cmd)
{
	int	exit_status;

	if (!validate_command(ctx, cmd))
		return (false);
	if (is_recognized_builtin(cmd->args[0]))
	{
		if (execute_builtin(ctx, cmd, &exit_status))
		{
			ctx->exit_status = exit_status;
			return (true);
		}
	}
	return (false);
}
