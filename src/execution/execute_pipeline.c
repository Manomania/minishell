/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipeline.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 16:45:37 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/05 16:15:39 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "error.h"
#include "execute.h"

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
	status = 0;
	while (i < cmd_count)
	{
		waitpid(pids[i], &status, 0);
		if (i == cmd_count - 1)
		{
			if (WIFEXITED(status))
				ctx->exit_status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
			{
				ctx->exit_status = 128 + WTERMSIG(status);
				if (WTERMSIG(status) == SIGQUIT)
					write(STDOUT_FILENO, "\n", 1);
			}
		}
		i++;
	}
}

/**
 * @brief Initialize pipeline execution
 *
 * Sets up initial variables and allocates memory for pids
 *
 * @param cmd Command list
 * @param pids Pointer to store allocated pid array
 * @return int Number of commands, or -1 if allocation failed
 */
static int	initialize_pipeline(t_command *cmd, pid_t **pids)
{
	int	cmd_count;

	cmd_count = count_commands(cmd);
	*pids = (pid_t *)malloc(sizeof(pid_t) * cmd_count);
	if (!*pids)
		return (-1);
	setup_parent_signals();
	return (cmd_count);
}

/**
 * @brief Process single command in pipeline
 *
 * Handles pipe creation, forking, and execution for one command
 *
 * @param current Current command to process
 * @param prev_read Previous pipe read fd
 * @param pipe_fds Current pipe fds array
 * @return int Returns updated prev_read, or -2 on error
 */
static int	process_command(t_command *current, int prev_read, int pipe_fds[2])
{
	if (!setup_command_pipe(pipe_fds, current->next != NULL))
		return (-2);
	if (current->next)
		return (pipe_fds[0]);
	return (prev_read);
}

/**
 * @brief Execute and clean up command
 *
 * Forks and executes a command, then cleans up pipes
 *
 * @param ctx Shell context
 * @param args Execution arguments
 * @return int 1 on success, 0 on error
 */
static int	execute_and_cleanup(t_ctx *ctx, t_eac_args args)
{
	if (!handle_command_fork(ctx, (t_hcf_args){args.current, args.prev_read,
			args.pipe_fds, args.pids, args.i}))
		return (0);
	cleanup_pipes(args.prev_read, args.pipe_fds, args.current->next != NULL);
	return (1);
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
	t_eac_args	args;
	int			cmd_count;
	int			result;

	cmd_count = initialize_pipeline(cmd, &args.pids);
	if (cmd_count == -1)
		return ;
	args.current = cmd;
	args.prev_read = -1;
	args.i = 0;
	while (args.current)
	{
		result = process_command(args.current, args.prev_read, args.pipe_fds);
		if (result == -2)
			return (handle_fork_error(args.pids, cmd_count, args.pipe_fds,
					args.prev_read));
		if (!execute_and_cleanup(ctx, args))
			return ;
		args.prev_read = result;
		args.current = args.current->next;
		args.i++;
	}
	wait_for_children(ctx, args.pids, cmd_count);
	free(args.pids);
	setup_signals();
}
