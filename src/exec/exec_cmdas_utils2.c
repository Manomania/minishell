/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmdas_utils2.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 15:52:23 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/21 17:11:03 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"
#include "builtins.h"

/**
 * Creates a pipe for the next command
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
 * Counts the number of commands in a pipeline
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
 * Reports command not found error with proper formatting
 *
 * @param cmd_name Name of the command that was not found
 */
void	report_cmd_not_found_pipe(char *cmd_name)
{
	ft_printf_fd(STDERR_FILENO, "minishell: %s: command not found\n", cmd_name);
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
	bin_path = validate_and_resolve_command(ctx, cmd);
	if (!bin_path && (!cmd->args || !cmd->args[0]))
	{
		cleanup_child_process(ctx);
		exit(EXIT_SUCCESS);
	}
	else if (!bin_path)
	{
		cleanup_child_process(ctx);
		exit(127);
	}
	free(cmd->args[0]);
	cmd->args[0] = bin_path;
	if (execve(cmd->args[0], cmd->args, ctx->envp) == -1)
		ctx_error_exit(ctx, cmd->args[0], "exec", ERR_CHILD);
}

/**
 * Sets up the child process for command execution
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
