/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipeline.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 16:45:37 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/02 17:41:44 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "error.h"
#include "execute.h"

/**
 * @brief Executes a command in a child process
 *
 * Handles redirections and executes the command
 *
 * @param ctx Shell context
 * @param cmd Command to execute
 * @param fds Input and output file descriptors
 * @param pids Process ids pointer to free
 */
static void	execute_command_in_child(t_ctx *ctx, t_command *cmd, t_fds fds,
		int *pids)
{
	char	*bin_path;
	int		bruh;
	int		status;

	if (fds.in != -1)
	{
		if (dup2(fds.in, STDIN_FILENO) == -1)
			exit(1);
		close(fds.in);
	}
	if (fds.out != -1)
	{
		if (dup2(fds.out, STDOUT_FILENO) == -1)
			exit(1);
		close(fds.out);
	}
	if (!apply_redirections(cmd))
	{
		ctx_clear(ctx);
		free(pids);
		exit(1);
	}
	if (is_builtin_command(cmd->args[0]))
	{
		bruh = execute_builtin(ctx, cmd);
		ctx_clear(ctx);
		free(pids);
		exit(bruh);
	}
	else
	{
		reset_signals();
		bin_path = bin_find(ctx, cmd->args[0]);
		if (!bin_path)
		{
			status = ctx->exit_status;
			ctx_clear(ctx);
			free(pids);
			exit(status);
		}
		execve(bin_path, cmd->args, ctx->envp);
		free(bin_path);
		exit(error(cmd->args[0], NULL, ERR_CMD_NOT_FOUND));
	}
}

/**
 * @brief Counts commands in a pipeline
 *
 * @param cmd Command list
 * @return int Number of commands
 */
static int	count_commands(t_command *cmd)
{
	t_command	*current;
	int			count;

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
 * @brief Cleans up pipe file descriptors
 *
 * @param prev_pipe_read Previous pipe read fd
 * @param pipe_fds Current pipe fds
 * @param has_next Whether there's a next command
 */
static void	cleanup_pipes(int prev_pipe_read, int pipe_fds[2], int has_next)
{
	if (prev_pipe_read != -1)
		close(prev_pipe_read);
	if (has_next)
	{
		close(pipe_fds[1]);
		prev_pipe_read = pipe_fds[0];
	}
}

/**
 * @brief Waits for child processes to finish
 *
 * @param ctx Shell context
 * @param pids Array of process IDs
 * @param cmd_count Number of commands
 */
static void	wait_for_children(t_ctx *ctx, pid_t *pids, int cmd_count)
{
	int	i;
	int	status;

	i = 0;
	while (i < cmd_count)
	{
		waitpid(pids[i], &status, 0);
		if (i == cmd_count - 1)
		{
			if (WIFEXITED(status))
				ctx->exit_status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				ctx->exit_status = 128 + WTERMSIG(status);
		}
		i++;
	}
}

/**
 * @brief Creates a pipe
 *
 * @param pipe_fds Array to store pipe file descriptors
 * @return t_bool true on success, false on error
 */
static t_bool	create_pipe(int pipe_fds[2])
{
	if (pipe(pipe_fds) == -1)
		return (error("pipe", NULL, ERR_PIPE), false);
	return (true);
}

/**
 * @brief Executes a pipeline of commands
 *
 * Creates pipes, forks processes, and waits for completion
 *
 * @param ctx Shell context
 * @param cmd Command list
 */
void	execute_pipeline(t_ctx *ctx, t_command *cmd)
{
	t_command	*current;
	int			pipe_fds[2];
	int			prev_pipe_read;
	pid_t		*pids;
	int			cmd_count;
	int			i;

	cmd_count = count_commands(cmd);
	pids = (pid_t *)malloc(sizeof(pid_t) * cmd_count);
	if (!pids)
		return ;
	current = cmd;
	prev_pipe_read = -1;
	i = 0;
	setup_parent_signals();
	while (current)
	{
		if (current->next && !create_pipe(pipe_fds))
		{
			i = 0;
			while (i < cmd_count && pids[i] > 0)
				kill(pids[i++], SIGKILL);
			free(pids);
			return ;
		}
		pids[i] = fork();
		if (pids[i] == -1)
		{
			if (current->next)
			{
				close(pipe_fds[0]);
				close(pipe_fds[1]);
			}
			if (prev_pipe_read != -1)
				close(prev_pipe_read);
			i = 0;
			while (i < cmd_count && pids[i] > 0)
				kill(pids[i++], SIGKILL);
			free(pids);
			return ;
		}
		if (pids[i] == 0)
		{
			if (current->next)
				close(pipe_fds[0]);
			execute_command_in_child(ctx, current, (t_fds){prev_pipe_read,
				(current->next ? pipe_fds[1] : -1)}, pids);
		}
		cleanup_pipes(prev_pipe_read, pipe_fds, current->next != NULL);
		if (current->next)
			prev_pipe_read = pipe_fds[0];
		current = current->next;
		i++;
	}
	wait_for_children(ctx, pids, cmd_count);
	free(pids);
	setup_signals();
}
