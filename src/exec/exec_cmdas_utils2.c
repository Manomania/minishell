/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmdas_utils2.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 15:52:23 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/15 11:41:35 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "debug.h"
#include "error.h"
#include "minishell.h"

/**
 * @brief Creates a pipe for the next command
 *
 * @param pipe_fds Array to store pipe file descriptors
 * @return int 0 on success, -1 on error
 */
int	setup_pipe(int pipe_fds[2])
{
	if (pipe(pipe_fds) == -1)
	{
		perror("pipe");
		return (-1);
	}
	return (0);
}

/**
 * @brief Counts the number of commands in a pipeline
 *
 * @param cmd The first command in the pipeline
 * @return int The number of commands
 */
int	count_commands(t_command *cmd)
{
	int			count;
	t_command	*current;

	count = 0;
	current = cmd;
	while (current)
	{
		count++;
		current = current->next;
	}
	return (count);
}

/**
 * @brief Reports command not found error with proper formatting
 *
 * @param cmd_name Name of the command that was not found
 */
static void	report_cmd_not_found_pipe(char *cmd_name)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	ft_putstr_fd(cmd_name, STDERR_FILENO);
	ft_putstr_fd(": command not found\n", STDERR_FILENO);
}

/**
 * @brief Handles command execution after redirection setup
 *
 * @param ctx Context with environment
 * @param cmd Command to execute
 */
void	execute_command(t_ctx *ctx, t_command *cmd)
{
	char	*bin_path;

	if (builtins_try(ctx, cmd))
	{
		cleanup_child_process(ctx);
		exit(EXIT_SUCCESS);
	}
	if (!cmd->args || !cmd->args[0])
	{
		cleanup_child_process(ctx);
		exit(EXIT_SUCCESS);
	}
	bin_path = bin_find(ctx, cmd->args[0]);
	if (!bin_path)
	{
		if (cmd->args[0][0] == '.' && !ft_strchr(cmd->args[0], '/'))
			ctx_error_exit(ctx, NULL, "exec", ERR_NO_FILE);
		if (ft_strchr(cmd->args[0], '/'))
			ctx_error_exit(ctx, cmd->args[0], "exec", ERR_IS_DIR);
		report_cmd_not_found_pipe(cmd->args[0]);
		cleanup_child_process(ctx);
		exit(127);
	}
	free(cmd->args[0]);
	cmd->args[0] = bin_path;
	if (execve(cmd->args[0], cmd->args, ctx->envp) == -1)
		ctx_error_exit(ctx, cmd->args[0], "exec", ERR_CHILD);
}

/**
 * @brief Sets up the child process for command execution
 *
 * @param ctx Context with environment
 * @param cmd Command to execute
 * @param input_fd Input file descriptor
 * @param output_fd Output file descriptor
 */
void	setup_child_process(t_ctx *ctx, t_command *cmd, int input_fd,
		int output_fd)
{
	int	result;

	reset_signals();
	result = setup_child_pipeline_redirections(cmd, input_fd, output_fd);
	if (result != 0)
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
	execute_command(ctx, cmd);
}
