/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_try.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 12:19:14 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/21 10:00:31 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "debug.h"
#include "error.h"
#include "minishell.h"

/**
 * @brief Compares a command name with a potential built-in
 *
 * @param cmd_name Command name to check
 * @param builtin_name Built-in name to compare against
 * @return int 0 if they match, non-zero otherwise
 */
static int	is_builtin(char *cmd_name, char *builtin_name)
{
	if (!cmd_name || !builtin_name)
		return (1);
	return (ft_strncmp(cmd_name, builtin_name, __INT_MAX__));
}

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
	int	result;

	saved_fds[0] = dup(STDIN_FILENO);
	saved_fds[1] = dup(STDOUT_FILENO);
	if (saved_fds[0] == -1 || saved_fds[1] == -1)
		return (ctx_error(ERR_IO_ERROR), false);
	result = builtin_setup_redirections(cmd, saved_fds);
	if (result == -1)
	{
		close(saved_fds[0]);
		close(saved_fds[1]);
		return (false);
	}
	if (is_builtin(cmd->args[0], (char *)"exit") == 0)
		*exit_status = builtin_exit(ctx, cmd);
	else if (is_builtin(cmd->args[0], (char *)"echo") == 0)
		*exit_status = builtin_echo(ctx, cmd);
	else if (is_builtin(cmd->args[0], (char *)"cd") == 0)
		*exit_status = builtin_cd(ctx, cmd);
	else if (is_builtin(cmd->args[0], (char *)"pwd") == 0)
		*exit_status = builtin_pwd(ctx, cmd);
	else if (is_builtin(cmd->args[0], (char *)"export") == 0)
		*exit_status = builtin_export(ctx, cmd);
	else if (is_builtin(cmd->args[0], (char *)"unset") == 0)
		*exit_status = builtin_unset(ctx, cmd);
	else if (is_builtin(cmd->args[0], (char *)"env") == 0)
		*exit_status = builtin_env(ctx, cmd);
	builtin_restore_redirections(saved_fds);
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
	char	cmd_name[64];
	int		exit_status;

	if (!ctx || !cmd || !cmd->args || !cmd->args[0])
	{
		error_print(ERROR, "builtin", "Invalid command");
		return (false);
	}
	ft_strlcpy(cmd_name, "Checking builtin: ", sizeof(cmd_name));
	ft_strlcat(cmd_name, cmd->args[0], sizeof(cmd_name));
	debug_log(DEBUG_INFO, "builtin", cmd_name);
	if (is_builtin(cmd->args[0], (char *)"exit") == 0
		|| is_builtin(cmd->args[0], (char *)"echo") == 0
		|| is_builtin(cmd->args[0], (char *)"cd") == 0
		|| is_builtin(cmd->args[0], (char *)"pwd") == 0
		|| is_builtin(cmd->args[0], (char *)"export") == 0
		|| is_builtin(cmd->args[0], (char *)"unset") == 0
		|| is_builtin(cmd->args[0], (char *)"env") == 0)
	{
		debug_log(DEBUG_INFO, "builtin", "Found builtin command");
		if (execute_builtin(ctx, cmd, &exit_status))
		{
			ctx->exit_status = exit_status;
			return (true);
		}
	}
	debug_log(DEBUG_INFO, "builtin", "No matching builtin found");
	return (false);
}
