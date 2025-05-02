/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipeline_setup.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 12:32:25 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/02 13:59:46 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Handles setup of a pipe for the pipeline
 *
 * @param pipe_fds Array to hold pipe file descriptors
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
	result = setup_child_pipeline_redirections(cmd, input_fd, output_fd, ctx);
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

/**
 * @brief Sets up child process redirections for pipeline
 *
 * @param cmd Command with redirections
 * @param input_fd Input file descriptor
 * @param output_fd Output file descriptor
 * @param ctx Context for exit status
 * @return int 0 on success, -1 on error
 */
int	setup_pipeline_redirections(t_command *cmd, int input_fd, int output_fd,
		t_ctx *ctx)
{
	if (input_fd != STDIN_FILENO)
	{
		if (dup2(input_fd, STDIN_FILENO) == -1)
		{
			close(input_fd);
			if (output_fd != STDOUT_FILENO)
				close(output_fd);
			return (-1);
		}
		close(input_fd);
	}
	if (output_fd != STDOUT_FILENO)
	{
		if (dup2(output_fd, STDOUT_FILENO) == -1)
		{
			if (input_fd != STDIN_FILENO)
				close(input_fd);
			close(output_fd);
			return (-1);
		}
		close(output_fd);
	}
	return (setup_redirections(cmd->redirection, ctx));
}
