/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmdas.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/30 16:37:25 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/14 14:33:13 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
 * @brief Sets up the child process for command execution
 *
 * @param ctx Context with environment
 * @param cmd Command to execute
 * @param input_fd Input file descriptor
 * @param output_fd Output file descriptor
 */
static void	setup_child_process(t_ctx *ctx, t_command *cmd, int input_fd,
		int output_fd)
{
	char	*bin_path;

	/* Reset signals to default for child process */
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
 * @brief Executes a single command in the pipeline
 *
 * @param ctx Context with environment
 * @param cmd Current command to execute
 * @param input_fd Input file descriptor
 * @param output_fd Output file descriptor
 * @return pid_t Process ID of the child
 */
static pid_t	exec_piped_command(t_ctx *ctx, t_command *cmd, int input_fd,
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
static int	setup_pipe(int pipe_fds[2])
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
static int	wait_for_pids(pid_t *pids, int count)
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

/**
 * @brief Execute one command in the pipeline process
 *
 * @param ctx Context information
 * @param current Current command being processed
 * @param pipe_fds Pipe file descriptors
 * @param i Index of current command
 * @param cmd_count Total number of commands
 * @param pids Array of process IDs
 * @param prev_pipe Previous pipe's read end
 * @return int Updated previous pipe file descriptor
 */
static int	process_pipeline_cmd(t_ctx *ctx, t_command *current,
		int pipe_fds[2], int i, int cmd_count, pid_t *pids, int prev_pipe)
{
	if (i < cmd_count - 1)
	{
		if (setup_pipe(pipe_fds) == -1)
			return (-1);
	}
	else
		pipe_fds[1] = STDOUT_FILENO;
	setup_parent_signals();
	pids[i] = exec_piped_command(ctx, current, prev_pipe, pipe_fds[1]);
	if (prev_pipe != STDIN_FILENO)
		close(prev_pipe);
	if (pipe_fds[1] != STDOUT_FILENO)
		close(pipe_fds[1]);
	if (i < cmd_count - 1)
		return (pipe_fds[0]);
	return (prev_pipe);
}

/**
 * @brief Execute the commands in a pipeline
 *
 * @param ctx Context
 * @return int Exit status of the last command
 */
t_bool	exec_cmdas(t_ctx *ctx)
{
	t_command	*current;
	int			cmd_count;
	int			pipe_fds[2];
	int			prev_pipe;
	int			i;
	pid_t		*pids;
	int			exit_status;

	current = ctx->cmd;
	cmd_count = count_commands(current);
	pids = malloc(sizeof(pid_t) * cmd_count);
	if (!pids)
		return (ctx_error(ERR_ALLOC));
	setup_parent_signals();
	prev_pipe = STDIN_FILENO;
	i = 0;
	while (i < cmd_count)
	{
		prev_pipe = process_pipeline_cmd(ctx, current, pipe_fds, i, cmd_count,
				pids, prev_pipe);
		if (prev_pipe == -1)
		{
			free(pids);
			return (ctx_error(ERR_PIPE));
		}
		current = current->next;
		i++;
	}
	exit_status = wait_for_pids(pids, cmd_count);
	setup_signals();
	free(pids);
	return (exit_status);
}
