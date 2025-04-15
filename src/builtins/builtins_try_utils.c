/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_try_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 11:43:56 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/15 15:55:50 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "minishell.h"

/**
 * @brief Compares a command name with a potential built-in
 *
 * @param cmd_name Command name to check
 * @param builtin_name Built-in name to compare against
 * @return int 0 if they match, non-zero otherwise
 */
int	is_builtin(char *cmd_name, char *builtin_name)
{
	if (!cmd_name || !builtin_name)
		return (1);
	return (ft_strncmp(cmd_name, builtin_name, __INT_MAX__));
}

/**
 * @brief Sets up redirections for builtin command execution
 *
 * @param cmd Command to set up redirections for
 * @param saved_fds Array of saved file descriptors
 * @return int 0 on success, -1 on failure
 */
int	setup_builtin_redirections(t_command *cmd, int *saved_fds)
{
	int	result;

	result = builtin_setup_redirections(cmd, saved_fds);
	if (result == -1)
	{
		close(saved_fds[0]);
		saved_fds[0] = -1;
		close(saved_fds[1]);
		saved_fds[1] = -1;
		return (-1);
	}
	return (0);
}

/**
 * @brief Executes the appropriate builtin command
 *
 * @param ctx Context for shell environment
 * @param cmd Command to execute
 * @param exit_status Pointer to store exit status
 */
void	run_builtin_command(t_ctx *ctx, t_command *cmd, int *exit_status)
{
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
}
