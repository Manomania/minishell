/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_pipeline_utils2.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maximart <maximart@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 18:14:22 by maximart          #+#    #+#             */
/*   Updated: 2025/04/21 18:14:31 by maximart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @brief Sets up file descriptors for a built-in in a pipeline
 *
 * @param fds Array containing [input_fd, output_fd]
 * @param saved_fds Array to store saved original fds
 * @return int 0 on success, error code otherwise
 */

#include "builtins.h"
#include "error.h"
#include "minishell.h"

int	setup_builtin_pipeline_fds(int *fds, int saved_fds[2])
{
	saved_fds[0] = dup(STDIN_FILENO);
	saved_fds[1] = dup(STDOUT_FILENO);
	if (saved_fds[0] == -1 || saved_fds[1] == -1)
	{
		if (saved_fds[0] != -1)
			close(saved_fds[0]);
		if (saved_fds[1] != -1)
			close(saved_fds[1]);
		return (ERR_IO);
	}
	if (fds[0] != STDIN_FILENO && dup2(fds[0], STDIN_FILENO) == -1)
	{
		restore_pipeline_fds(saved_fds[0], saved_fds[1]);
		return (ERR_FD);
	}
	if (fds[1] != STDOUT_FILENO && dup2(fds[1], STDOUT_FILENO) == -1)
	{
		restore_pipeline_fds(saved_fds[0], saved_fds[1]);
		return (ERR_FD);
	}
	return (0);
}

/**
 * @brief Executes the appropriate built-in command
 *
 * @param ctx Shell context
 * @param cmd Command to execute
 * @return int Exit status of the command
 */
int	execute_builtin_command(t_ctx *ctx, t_command *cmd)
{
	int	exit_status;

	if (ft_strncmp(cmd->args[0], "exit", __INT_MAX__) == 0)
	{
		exit_status = builtin_exit(ctx, cmd);
		ctx->exit_requested = false;
	}
	else if (ft_strncmp(cmd->args[0], "echo", __INT_MAX__) == 0)
		exit_status = builtin_echo(ctx, cmd);
	else if (ft_strncmp(cmd->args[0], "cd", __INT_MAX__) == 0)
		exit_status = builtin_cd(ctx, cmd);
	else if (ft_strncmp(cmd->args[0], "pwd", __INT_MAX__) == 0)
		exit_status = builtin_pwd(ctx, cmd);
	else if (ft_strncmp(cmd->args[0], "export", __INT_MAX__) == 0)
		exit_status = builtin_export(ctx, cmd);
	else if (ft_strncmp(cmd->args[0], "unset", __INT_MAX__) == 0)
		exit_status = builtin_unset(ctx, cmd);
	else if (ft_strncmp(cmd->args[0], "env", __INT_MAX__) == 0)
		exit_status = builtin_env(ctx, cmd);
	else
		exit_status = 127;
	return (exit_status);
}

/**
 * @brief Runs a built-in command with redirections
 *
 * @param ctx Context containing environment
 * @param cmd Command to execute
 * @return int Exit status of command
 */
int	run_builtin_with_redir(t_ctx *ctx, t_command *cmd)
{
	int	builtin_redir_fds[2];
	int	exit_status;

	builtin_redir_fds[0] = -1;
	builtin_redir_fds[1] = -1;
	if (setup_builtin_redirections(cmd, builtin_redir_fds) != 0)
		exit_status = 1;
	else
		exit_status = execute_builtin_command(ctx, cmd);
	builtin_restore_redirections(builtin_redir_fds);
	return (exit_status);
}

/**
 * @brief Restores all file descriptors to original state
 *
 * @param saved_fds Array with saved descriptors
 */
void	restore_all_fds(int saved_fds[2])
{
	restore_pipeline_fds(saved_fds[0], saved_fds[1]);
}

/**
 * @brief Prepares context and executes built-in in pipeline
 *
 * @param ctx Shell context
 * @param cmd Command to execute
 * @param fds Array containing input_fd and output_fd
 * @return int Exit status of the built-in
 */
int	prepare_and_run_builtin(t_ctx *ctx, t_command *cmd, int *fds)
{
	t_ctx	temp_ctx;
	t_env	*temp_env;
	int		result;
	int		saved_fds[2];
	int		exit_status;

	temp_env = duplicate_env_list(ctx->env_list);
	if (!temp_env)
		return (1);
	temp_ctx = *ctx;
	temp_ctx.env_list = temp_env;
	result = setup_builtin_pipeline_fds(fds, saved_fds);
	if (result != 0)
	{
		free_env_list(temp_env);
		return (error(NULL, "pipeline_builtin", result));
	}
	exit_status = run_builtin_with_redir(&temp_ctx, cmd);
	restore_all_fds(saved_fds);
	free_env_list(temp_env);
	return (exit_status);
}
