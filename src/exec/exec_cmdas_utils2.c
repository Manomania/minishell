/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmdas_utils2.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 15:52:23 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/31 13:24:56 by elagouch         ###   ########.fr       */
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
	int	new_read_fd;
	int	new_write_fd;

	if (pipe(pipe_fds) == -1)
	{
		perror("pipe");
		return (-1);
	}
	new_read_fd = dup2(pipe_fds[0], pipe_fds[0]);
	new_write_fd = dup2(pipe_fds[1], pipe_fds[1]);
	if (new_read_fd == -1 || new_write_fd == -1)
	{
		perror("dup2");
		close(pipe_fds[0]);
		close(pipe_fds[1]);
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
 * @brief Handles command execution after redirection setup
 *
 * @param ctx Context with environment
 * @param cmd Command to execute
 * @return void
 */
static void	execute_command(t_ctx *ctx, t_command *cmd)
{
	char	*bin_path;

	if (builtins_try(ctx, cmd))
		exit(EXIT_SUCCESS);
	if (!cmd->args || !cmd->args[0])
		exit(EXIT_FAILURE);
	bin_path = bin_find(ctx, cmd->args[0]);
	if (!bin_path)
		ctx_error_exit(ctx, cmd->args[0], "exec", ERR_CMD_NOT_FOUND);
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
	reset_signals();
	if (input_fd != STDIN_FILENO)
	{
		dup2(input_fd, STDIN_FILENO);
		close(input_fd);
	}
	if (output_fd != STDOUT_FILENO)
	{
		dup2(output_fd, STDOUT_FILENO);
		close(output_fd);
	}
	if (handle_redirections(cmd->redirection) != 0)
		exit(EXIT_FAILURE);
	execute_command(ctx, cmd);
}
