/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmdas_utils2.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 15:52:23 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/24 11:20:45 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "debug.h"
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
 * @brief Debug stff
 *
 * @param exit_status Exit status
 */
void	debug_exit_status_cmdas(int exit_status)
{
	char	*a;
	char	error_buf[64];

	ft_strlcpy(error_buf, "Pipeline exit status: ", sizeof(error_buf));
	a = ft_itoa(exit_status);
	ft_strlcat(error_buf, a, sizeof(error_buf));
	free(a);
	debug_log(DEBUG_INFO, "pipeline", error_buf);
}
