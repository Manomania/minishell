/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_pipeline.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 10:20:14 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/16 17:07:38 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "debug.h"
#include "error.h"
#include "minishell.h"

/**
 * @brief Checks if a command is a built-in
 *
 * @param cmd_name Command name to check
 * @return t_bool true if built-in, false otherwise
 */
t_bool	is_builtin_command(char *cmd_name)
{
	if (!cmd_name)
		return (false);
	if (ft_strncmp(cmd_name, "exit", __INT_MAX__) == 0)
		return (true);
	if (ft_strncmp(cmd_name, "echo", __INT_MAX__) == 0)
		return (true);
	if (ft_strncmp(cmd_name, "cd", __INT_MAX__) == 0)
		return (true);
	if (ft_strncmp(cmd_name, "pwd", __INT_MAX__) == 0)
		return (true);
	if (ft_strncmp(cmd_name, "export", __INT_MAX__) == 0)
		return (true);
	if (ft_strncmp(cmd_name, "unset", __INT_MAX__) == 0)
		return (true);
	if (ft_strncmp(cmd_name, "env", __INT_MAX__) == 0)
		return (true);
	return (false);
}

/**
 * @brief Executes a built-in command in a pipeline
 *
 * @param ctx Shell context
 * @param cmd Command to execute
 * @param input_fd Input file descriptor
 * @param output_fd Output file descriptor
 * @return int Exit status of the built-in
 */
static int	execute_pipeline_builtin(t_ctx *ctx, t_command *cmd,
									int *input_fd, int *output_fd)
{
	int	fds[2];

	fds[0] = *input_fd;
	fds[1] = *output_fd;
	return (prepare_and_run_builtin(ctx, cmd, fds));
}

/**
 * @brief Prepares redirection files before execution
 *
 * @param cmd Command with redirections
 * @return 0 on success, -1 on error
 */
static int	preprocess_redirections(t_command *cmd)
{
	t_redirection	*redir;
	int				fd;

	redir = cmd->redirection;
	while (redir)
	{
		if (redir->type == TOK_REDIR_TO || redir->type == TOK_HERE_DOC_TO)
		{
			fd = open_redirection_file(redir);
			if (fd == -1)
				return (-1);
			else
				close(fd);
		}
		redir = redir->next;
	}
	return (0);
}

/**
 * @brief Executes a command in a pipeline context
 *
 * @param ctx Shell context
 * @param cmd Command to execute
 * @param input_fd Pipeline input file descriptor
 * @param output_fd Pipeline output file descriptor
 * @return pid_t Child PID, -2 for built-in, -1 on error
 */
pid_t	execute_pipeline_command(t_ctx *ctx, t_command *cmd, int *input_fd,
		int *output_fd)
{
	pid_t	pid;
	int		status;

	if (!cmd->args || !cmd->args[0])
		return (-1);
	preprocess_redirections(cmd);
	if (is_builtin_command(cmd->args[0]))
	{
		status = execute_pipeline_builtin(ctx, cmd, input_fd, output_fd);
		ctx->exit_status = status;
		return (-2);
	}
	else
	{
		pid = fork();
		if (pid == -1)
		{
			perror("fork");
			return (-1);
		}
		if (pid == 0)
			setup_child_process(ctx, cmd, *input_fd, *output_fd);
		return (pid);
	}
}
