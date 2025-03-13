/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmdas.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/30 16:37:25 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/13 12:29:24 by elagouch         ###   ########.fr       */
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
 * @brief Resolves binary paths for a command
 *
 * @param ctx Context with environment
 * @param cmd Command to resolve path for
 * @return int 0 on success, -1 on failure
 */
static int	resolve_command_binary(t_ctx *ctx, t_command *cmd)
{
	char	*bin_og;
	char	*bin;

	if (!cmd->args || !cmd->args[0])
		return (-1);
	if (ft_strncmp(cmd->args[0], "cat", __INT_MAX__) == 0
		|| ft_strncmp(cmd->args[0], "ls", __INT_MAX__) == 0
		|| ft_strncmp(cmd->args[0], "grep", __INT_MAX__) == 0
		|| ft_strncmp(cmd->args[0], "echo", __INT_MAX__) == 0
		|| ft_strncmp(cmd->args[0], "wc", __INT_MAX__) == 0)
	{
		bin_og = cmd->args[0];
		bin = ft_strjoin("/bin/", bin_og);
		if (!bin)
			return (-1);
		free(bin_og);
		cmd->args[0] = bin;
		return (0);
	}
	bin_og = cmd->args[0];
	bin = bin_find(ctx, bin_og);
	if (!bin)
	{
		free(bin_og);
		cmd->args[0] = NULL;
		return (-1);
	}
	free(bin_og);
	cmd->args[0] = bin;
	return (0);
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
	{
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
		if (resolve_command_binary(ctx, cmd) != 0)
		{
			ft_printf("Command not found: %s\n", cmd->args[0]);
			exit(EXIT_FAILURE);
		}
		if (execve(cmd->args[0], cmd->args, ctx->envp) == -1)
		{
			perror("execve");
			exit(EXIT_FAILURE);
		}
	}
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
	prev_pipe = STDIN_FILENO;
	i = 0;
	while (i < cmd_count)
	{
		if (i < cmd_count - 1)
		{
			if (setup_pipe(pipe_fds) == -1)
			{
				free(pids);
				return (ctx_error(ERR_PIPE));
			}
		}
		else
			pipe_fds[1] = STDOUT_FILENO;
		pids[i] = exec_piped_command(ctx, current, prev_pipe, pipe_fds[1]);
		if (prev_pipe != STDIN_FILENO)
			close(prev_pipe);
		if (pipe_fds[1] != STDOUT_FILENO)
			close(pipe_fds[1]);
		if (i < cmd_count - 1)
			prev_pipe = pipe_fds[0];
		current = current->next;
		i++;
	}
	exit_status = wait_for_pids(pids, cmd_count);
	free(pids);
	return (exit_status);
}
