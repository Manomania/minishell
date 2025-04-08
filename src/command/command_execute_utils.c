/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_execute_utils.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 17:54:30 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/08 14:48:05 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "debug.h"
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
static int	setup_child_redirections(t_ctx *ctx, t_command *cmd)
{
	int	result;

	result = setup_heredocs(ctx, cmd);
	if (result != 0)
		return (result);
	result = setup_redirections(cmd->redirection);
	return (result);
}

/**
 * @brief Clean up all resources before child process exit
 *
 * @param ctx Context to clean up
 */
static void	child_process_cleanup(t_ctx *ctx)
{
	cleanup_heredoc_resources(ctx);
	if (ctx->tokens)
	{
		free_all_token(ctx->tokens);
		ctx->tokens = NULL;
	}
	ctx_clear(ctx);
}

t_bool	has_only_redirections(t_command *cmd)
{
	return (cmd && cmd->redirection && (!cmd->args || !cmd->args[0]));
}

/**
 * @brief Executes command or processes redirections if no command
 *
 * @param ctx Context with environment
 * @param cmd Command to execute
 */
static void	execute_command_or_redir(t_ctx *ctx, t_command *cmd)
{
	if (!cmd->args || !cmd->args[0])
	{
		child_process_cleanup(ctx);
		exit(EXIT_SUCCESS);
	}
	execve(cmd->args[0], cmd->args, ctx->envp);
	perror("execve");
	child_process_cleanup(ctx);
	exit(EXIT_FAILURE);
}

/**
 * @brief Executes the child process portion of a single command
 *
 * This function sets up redirections first, then checks if the command
 * exists,
	ensuring that redirections are applied even for non-existent commands.
 *
 * @param ctx Context containing environment and command info
 */
void	execute_child(t_ctx *ctx)
{
	int	redirect_result;

	reset_signals();
	redirect_result = setup_child_redirections(ctx, ctx->cmd);
	if (redirect_result != 0)
	{
		child_process_cleanup(ctx);
		exit(EXIT_FAILURE);
	}
	if (!ctx->cmd->args || !ctx->cmd->args[0])
	{
		child_process_cleanup(ctx);
		exit(EXIT_SUCCESS);
	}
	if (!command_bin(ctx))
	{
		if (ctx->cmd->args && ctx->cmd->args[0])
		{
			if (ctx->cmd->args[0][0] == '.' && !ft_strchr(ctx->cmd->args[0],
					'/'))
				exit(error_code(ERR_NO_FILE));
			if (ft_strchr(ctx->cmd->args[0], '/'))
				exit(error_code(ERR_IS_DIR));
			exit(error(ctx->cmd->args[0], "exec", ERR_CMD_NOT_FOUND));
		}
		exit(error_code(ERR_CMD_NOT_FOUND));
	}
	execute_command_or_redir(ctx, ctx->cmd);
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
