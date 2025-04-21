/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmdas.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/30 16:37:25 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/21 17:57:34 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"

#include "error.h"
#include "memory.h"
#include "minishell.h"

/**
 * Handles pipe setup for commands
 *
 * @param pipe_fds Pipe file descriptors array to set up
 * @param i Current command index
 * @param cmd_count Total command count
 * @return 0 on success, -1 on error
 */
int	handle_pipe_setup(int pipe_fds[2], int i, int cmd_count)
{
	if (i < cmd_count - 1)
	{
		if (pipe(pipe_fds) == -1)
			return (-1);
	}
	else
	{
		pipe_fds[0] = -1;
		pipe_fds[1] = STDOUT_FILENO;
	}
	return (0);
}

/**
 * Processes redirections for a command before forking
 *
 * @param cmd Command with redirections
 * @return 0 on success, -1 on error
 */
static int	handle_pre_fork_redirections(t_command *cmd)
{
	t_redirection	*redir;
	int				fd;

	if (!cmd || !cmd->redirection)
		return (0);
	redir = cmd->redirection;
	while (redir)
	{
		if (redir->type == TOK_REDIR_TO || redir->type == TOK_HERE_DOC_TO)
		{
			fd = open_redirection_file(redir);
			if (fd != -1)
			{
				write(fd, "", 0);
				close(fd);
			}
		}
		redir = redir->next;
	}
	return (0);
}

/**
 * Handles descriptor management for pipeline execution
 *
 * @param prev_pipe Previous pipe's read end
 * @param pipe_fds Current pipe file descriptors
 * @param i Current command index
 * @param cmd_count Total command count
 * @return Updated previous pipe file descriptor
 */
int	handle_descriptors(int prev_pipe, int pipe_fds[2], int i, int cmd_count)
{
	int	next_pipe;

	next_pipe = STDIN_FILENO;
	if (prev_pipe > 2)
		close(prev_pipe);
	if (i < cmd_count - 1)
	{
		next_pipe = pipe_fds[0];
		if (pipe_fds[1] > 2)
			close(pipe_fds[1]);
	}
	else
	{
		if (pipe_fds[0] > 2)
			close(pipe_fds[0]);
	}
	return (next_pipe);
}

/**
 * Cleanup pipe file descriptors in error cases
 *
 * @param pipe_fds Pipe file descriptors array
 */
void	cleanup_pipe_fds(int pipe_fds[2])
{
	if (pipe_fds[0] != -1 && pipe_fds[0] != STDIN_FILENO)
		close(pipe_fds[0]);
	if (pipe_fds[1] != -1 && pipe_fds[1] != STDOUT_FILENO)
		close(pipe_fds[1]);
	pipe_fds[0] = -1;
	pipe_fds[1] = -1;
}

/**
 * Execute a command with redirections in the pipeline
 *
 * @param ctx Context information
 * @param data Pipe data structure
 * @param input_fd Input file descriptor
 * @param output_fd Output file descriptor
 * @return Process ID or -1 on error
 */
pid_t	execute_pipeline_cmd_with_redir(t_ctx *ctx, t_pipe_data *data,
		int input_fd, int output_fd)
{
	pid_t	pid;

	handle_pre_fork_redirections(data->current);
	if (has_only_redirections_pipeline(data->current))
	{
		pid = execute_redirections_only_pipeline(ctx, data);
		data->pids[data->i] = pid;
	}
	else
	{
		pid = execute_pipeline_command(ctx, data->current, &input_fd,
				&output_fd);
		data->pids[data->i] = pid;
	}
	return (pid);
}
