/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmdas_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 15:33:08 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/14 15:43:00 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Handles command execution after redirection setup
 *
 * @param ctx Context with environment
 * @param cmd Command to execute
 * @return void
 */
void	execute_command(t_ctx *ctx, t_command *cmd)
{
	char	*bin_path;

	if (builtins_try(ctx, cmd))
		exit(EXIT_SUCCESS);
	if (!cmd->args || !cmd->args[0])
		exit(EXIT_FAILURE);
	bin_path = bin_find(ctx, cmd->args[0]);
	if (!bin_path)
	{
		ft_printf("Command not found: %s\n", cmd->args[0]);
		exit(EXIT_FAILURE);
	}
	free(cmd->args[0]);
	cmd->args[0] = bin_path;
	if (execve(cmd->args[0], cmd->args, ctx->envp) == -1)
	{
		perror("execve");
		exit(EXIT_FAILURE);
	}
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

/**
 * @brief Executes a single command in the pipeline
 *
 * @param ctx Context with environment
 * @param cmd Current command to execute
 * @param input_fd Input file descriptor
 * @param output_fd Output file descriptor
 * @return pid_t Process ID of the child
 */
pid_t	exec_piped_command(t_ctx *ctx, t_command *cmd, int input_fd,
		int output_fd)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return (-1);
	}
	if (pid == 0)
		setup_child_process(ctx, cmd, input_fd, output_fd);
	return (pid);
}

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
 * @brief Waits for all child processes and reports status
 *
 * @param pids Array of process IDs
 * @param count Number of processes
 * @return int Exit status of the last command
 */
int	wait_for_pids(pid_t *pids, int count)
{
	int	i;
	int	status;
	int	last_status;

	i = 0;
	last_status = 0;
	while (i < count)
	{
		if (pids[i] > 0)
			waitpid(pids[i], &status, 0);
		if (i == count - 1 && pids[i] > 0)
		{
			if (WIFEXITED(status))
				last_status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				last_status = 128 + WTERMSIG(status);
		}
		i++;
	}
	return (last_status);
}
