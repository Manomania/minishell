/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_execute.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 16:10:59 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/14 14:56:39 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Executes the child process portion of a single command
 */
static void	execute_child(t_ctx *ctx)
{
	reset_signals();
	handle_redirections(ctx->cmd->redirection);
	if (!ctx->cmd->args[0] || access(ctx->cmd->args[0], X_OK) != 0)
	{
		ft_printf("Command not found or not executable: %s\n",
			ctx->cmd->args[0]);
		exit(EXIT_FAILURE);
	}
	execve(ctx->cmd->args[0], ctx->cmd->args, ctx->envp);
	perror("execve");
	exit(EXIT_FAILURE);
}

/**
 * @brief Gets the exit status from process termination information
 */
static int	get_exit_status(int status)
{
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}

/**
 * @brief Executes a single command without a pipeline
 */
static int	execute_single_command(t_ctx *ctx)
{
	pid_t	pid;
	int		status;

	if (builtins_try(ctx, ctx->cmd))
		return (0);
	if (!command_bin(ctx))
		return (ctx_error(ERR_CMD_NOT_FOUND));
	setup_parent_signals();
	pid = fork();
	if (pid == -1)
		return (ctx_error(ERR_CHILD));
	if (pid == 0)
		execute_child(ctx);
	waitpid(pid, &status, 0);
	setup_signals();
	return (get_exit_status(status));
}

/**
 * @brief Checks if the command is part of a pipeline
 *
 * @param cmd Command to check
 * @return t_bool true if command is part of a pipeline
 */
static t_bool	is_pipeline(t_command *cmd)
{
	return (cmd && cmd->next);
}

/**
 * @brief Executes a command or pipeline
 *
 * @param ctx Context containing environment
 * @return int 0 on success, -1 on error
 */
int	command_execute(t_ctx *ctx)
{
	if (!ctx || !ctx->cmd)
		return (ctx_error(ERR_ALLOC));
	if (!ctx->cmd->args || !ctx->cmd->args[0])
		return (-1);
	if (is_pipeline(ctx->cmd))
		return (exec_cmdas(ctx));
	return (execute_single_command(ctx));
}
