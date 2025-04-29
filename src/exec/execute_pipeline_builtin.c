/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipeline_builtin.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 12:31:50 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/29 13:42:01 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "minishell.h"

/**
 * @brief Saves the standard input and output file descriptors
 *
 * @param saved_fds Array to store the saved file descriptors
 * @return int 0 on success, 1 on failure
 */
static int	save_standard_fds(int saved_fds[2])
{
	saved_fds[0] = dup(STDIN_FILENO);
	saved_fds[1] = dup(STDOUT_FILENO);
	if (saved_fds[0] == -1 || saved_fds[1] == -1)
	{
		if (saved_fds[0] != -1)
			close(saved_fds[0]);
		if (saved_fds[1] != -1)
			close(saved_fds[1]);
		return (1);
	}
	return (0);
}

/**
 * @brief Sets up file descriptors for command execution in pipeline
 *
 * @param input_fd Input file descriptor
 * @param output_fd Output file descriptor
 */
static void	setup_pipeline_fds(int input_fd, int output_fd)
{
	if (input_fd != STDIN_FILENO)
		dup2(input_fd, STDIN_FILENO);
	if (output_fd != STDOUT_FILENO)
		dup2(output_fd, STDOUT_FILENO);
}

/**
 * @brief Restores the standard input and output file descriptors
 *
 * @param saved_fds Array containing the saved file descriptors
 */
static void	restore_standard_fds(int saved_fds[2])
{
	dup2(saved_fds[0], STDIN_FILENO);
	dup2(saved_fds[1], STDOUT_FILENO);
	close(saved_fds[0]);
	close(saved_fds[1]);
}

/**
 * @brief Executes builtin command in pipeline
 *
 * @param ctx Context containing shell state
 * @param cmd Command to execute
 * @param input_fd Input file descriptor
 * @param output_fd Output file descriptor
 * @return int Exit status code from builtin
 */
int	execute_pipeline_builtin(t_ctx *ctx, t_command *cmd, int input_fd,
		int output_fd)
{
	int	saved_fds[2];
	int	exit_status;

	if (save_standard_fds(saved_fds) != 0)
		return (1);
	setup_pipeline_fds(input_fd, output_fd);
	if (setup_command_redirections(ctx, cmd) != 0)
		exit_status = 1;
	else
	{
		builtins_try(ctx, cmd);
		exit_status = ctx->exit_status;
	}
	restore_standard_fds(saved_fds);
	return (exit_status);
}
