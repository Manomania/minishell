/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_execute_utils.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 17:54:30 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/02 14:08:23 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"

/**
 * @brief Sets up child process redirections
 *
 * This function handles heredocs and redirections in the child process.
 *
 * @param ctx Context with environment information
 * @param cmd Command containing redirections
 * @return 0 on success, non-zero on error
 */
int	setup_child_redirections(t_ctx *ctx, t_command *cmd)
{
	int	result;

	result = setup_heredocs(ctx, cmd);
	if (result != 0)
		return (result);
	result = setup_redirections(cmd->redirection, ctx);
	return (result);
}

/**
 * @brief Figures out if pipeline has only redirections
 *
 * @param cmd Command
 * @return t_bool Whether the pipeline has only redirections or not
 */
t_bool	has_only_redirections(t_command *cmd)
{
	return (cmd && cmd->redirection && (!cmd->args || !cmd->args[0]));
}

/**
 * @brief Gets the exit status from process termination information
 *
 * This function extracts the exit status from the process status.
 *
 * @param status Process status from waitpid
 * @return Exit status of the process
 */
int	get_exit_status(int status)
{
	int	exit_code;

	if (WIFEXITED(status))
	{
		exit_code = WEXITSTATUS(status);
		return (exit_code);
	}
	if (WIFSIGNALED(status))
	{
		exit_code = 128 + WTERMSIG(status);
		return (exit_code);
	}
	return (1);
}

/**
 * Sets up and manages child process redirection
 *
 * @param ctx Context containing shell state
 * @return int Status code, 0 for success or error code
 */
int	handle_child_redirections(t_ctx *ctx)
{
	int	redirect_result;

	redirect_result = setup_child_redirections(ctx, ctx->cmd);
	if (redirect_result != 0)
		return (EXIT_FAILURE);
	if (!ctx->cmd->args || !ctx->cmd->args[0])
		return (EXIT_SUCCESS);
	return (0);
}
