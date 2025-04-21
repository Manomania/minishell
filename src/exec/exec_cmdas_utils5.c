/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmdas_utils5.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 17:04:51 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/21 17:11:23 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"

/**
 * @brief Handles signal-related output after waiting for processes
 *
 * @param status The process status from waitpid
 * @return void
 */
void	handle_signal_output(int status)
{
	if (WTERMSIG(status) == SIGQUIT)
		ft_printf_fd(STDERR_FILENO, "Quit (core dumped)\n");
	else if (WTERMSIG(status) != SIGINT && isatty(STDOUT_FILENO))
		write(STDOUT_FILENO, "\n", 1);
}

/**
 * @brief Process the last command status in a pipeline
 *
 * @param status Process status from waitpid
 * @param was_signaled Flag indicating if signal occurred
 * @return int Processed exit status
 */
int	process_last_command_status(int status, int *was_signaled)
{
	int	last_status;

	last_status = 0;
	if (WIFEXITED(status))
		last_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
	{
		last_status = 128 + WTERMSIG(status);
		*was_signaled = 1;
		if (WTERMSIG(status) == SIGQUIT)
			ft_printf_fd(STDERR_FILENO, "Quit (core dumped)\n");
		else if (isatty(STDOUT_FILENO))
			write(STDOUT_FILENO, "\n", 1);
	}
	return (last_status);
}

/**
 * @brief Sets up pipe redirections for a child process
 *
 * @param data Pipeline data structure containing file descriptors
 * @return void
 */
void	setup_pipe_redirections(t_pipe_data *data)
{
	if (data->prev_pipe != STDIN_FILENO)
	{
		dup2(data->prev_pipe, STDIN_FILENO);
		close(data->prev_pipe);
	}
	if (data->pipe_fds[1] != STDOUT_FILENO)
	{
		dup2(data->pipe_fds[1], STDOUT_FILENO);
		close(data->pipe_fds[1]);
	}
	if (data->pipe_fds[0] != STDIN_FILENO)
		close(data->pipe_fds[0]);
}

/**
 * @brief Validates command and resolves binary path
 *
 * @param ctx Context with environment
 * @param cmd Command to validate
 * @return char* The resolved binary path or NULL
 */
char	*validate_and_resolve_command(t_ctx *ctx, t_command *cmd)
{
	char	*bin_path;

	if (!cmd->args || !cmd->args[0])
		return (NULL);
	bin_path = bin_find(ctx, cmd->args[0]);
	if (!bin_path)
	{
		if (cmd->args[0][0] == '.' && !ft_strchr(cmd->args[0], '/'))
			ctx_error_exit(ctx, NULL, "exec", ERR_NO_FILE);
		if (ft_strchr(cmd->args[0], '/'))
			ctx_error_exit(ctx, cmd->args[0], "exec", ERR_IS_DIR);
		report_cmd_not_found_pipe(cmd->args[0]);
	}
	return (bin_path);
}
