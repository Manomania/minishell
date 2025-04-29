/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipeline_command.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 12:34:45 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/29 15:13:19 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "error.h"
#include "minishell.h"

/**
 * @brief Set up redirections and handle errors
 *
 * @param ctx Context containing shell environment
 * @param cmd Command to execute
 * @param input_fd Input file descriptor
 * @param output_fd Output file descriptor
 * @return int 0 on success, 1 on error
 */
int	epc_setup_pipeline_io(t_ctx *ctx, t_command *cmd, int input_fd,
		int output_fd)
{
	if (setup_pipeline_redirections(cmd, input_fd, output_fd) != 0)
	{
		cleanup_child_process(ctx);
		return (1);
	}
	if (setup_heredocs(ctx, cmd) != 0)
	{
		cleanup_child_process(ctx);
		return (1);
	}
	if (!cmd->args || !cmd->args[0])
	{
		cleanup_child_process(ctx);
		return (1);
	}
	return (0);
}

/**
 * @brief Handle execution of builtin commands
 *
 * @param ctx Context containing shell environment
 * @param cmd Command to execute
 * @return void
 */
void	epc_execute_builtin(t_ctx *ctx, t_command *cmd)
{
	builtins_try(ctx, cmd);
	cleanup_child_process(ctx);
	exit(ctx->exit_status);
}

/**
 * @brief Handle command not found errors
 *
 * @param ctx Context containing shell environment
 * @param cmd Command that was not found
 * @return void
 */
void	epc_handle_command_not_found(t_ctx *ctx, t_command *cmd)
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

/**
 * @brief Execute the command using execve
 *
 * @param ctx Context containing shell environment
 * @param cmd Command to execute
 * @param bin_path Path to the binary
 * @return void
 */
void	epc_execute_external_command(t_ctx *ctx, t_command *cmd, char *bin_path)
{
	char	**env_array;

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
	char	*bin_path;
	int		error_code;

	reset_signals();
	error_code = epc_setup_pipeline_io(ctx, cmd, input_fd, output_fd);
	if (error_code == 1)
		exit(EXIT_FAILURE);
	else if (error_code)
		exit(EXIT_SUCCESS);
	if (is_builtin_command(cmd->args[0]))
		epc_execute_builtin(ctx, cmd);
	bin_path = bin_find(ctx, cmd->args[0]);
	if (!bin_path)
		epc_handle_command_not_found(ctx, cmd);
	epc_execute_external_command(ctx, cmd, bin_path);
}
