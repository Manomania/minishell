/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmdas_utils5.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 17:04:51 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/28 13:31:38 by elagouch         ###   ########.fr       */
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

/**
 * @brief Handles the last process's status
 *
 * @param pids Array of process IDs
 * @param status Status of the process
 * @param i Current process index
 * @return int Exit status code
 */
int	handle_last_process_status(pid_t *pids, int status, int i)
{
	int	last_status;

	last_status = 0;
	if (pids[i] > 0)
	{
		if (WIFEXITED(status))
			last_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			last_status = 128 + WTERMSIG(status);
	}
	else if (pids[i] == -2)
		last_status = 0;
	return (last_status);
}
