/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipeline_command.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 12:34:45 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/29 13:40:14 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "error.h"
#include "minishell.h"

/**
 * @brief Executes a command in child process
 *
 * @param ctx Context containing shell environment
 * @param cmd Command to execute
 * @param input_fd Input file descriptor
 * @param output_fd Output file descriptor
 */
void	execute_pipeline_command(t_ctx *ctx, t_command *cmd, int input_fd,
		int output_fd)
{
	char	**env_array;
	char	*bin_path;

	reset_signals();
	if (setup_pipeline_redirections(cmd, input_fd, output_fd) != 0)
	{
		cleanup_child_process(ctx);
		exit(EXIT_FAILURE);
	}
	if (setup_heredocs(ctx, cmd) != 0)
	{
		cleanup_child_process(ctx);
		exit(EXIT_FAILURE);
	}
	if (!cmd->args || !cmd->args[0])
	{
		cleanup_child_process(ctx);
		exit(EXIT_SUCCESS);
	}
	if (is_builtin_command(cmd->args[0]))
	{
		builtins_try(ctx, cmd);
		cleanup_child_process(ctx);
		exit(ctx->exit_status);
	}
	bin_path = bin_find(ctx, cmd->args[0]);
	if (!bin_path)
	{
		if (cmd->args[0][0] == '.' && !ft_strchr(cmd->args[0], '/'))
			exit(error(NULL, "exec", ERR_NO_FILE));
		if (ft_strchr(cmd->args[0], '/'))
			exit(error(cmd->args[0], NULL, ERR_IS_DIR));
		ft_printf_fd(STDERR_FILENO, "minishell: %s: command not found\n",
			cmd->args[0]);
		cleanup_child_process(ctx);
		exit(127);
	}
	free(cmd->args[0]);
	cmd->args[0] = bin_path;
	env_array = create_env_array(ctx->env_list);
	if (!env_array)
	{
		cleanup_child_process(ctx);
		exit(EXIT_FAILURE);
	}
	execve(cmd->args[0], cmd->args, env_array);
	perror("execve");
	free_2d_array((void **)env_array);
	cleanup_child_process(ctx);
	exit(EXIT_FAILURE);
}
