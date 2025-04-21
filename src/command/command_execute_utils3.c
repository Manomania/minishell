/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_execute_utils3.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 16:20:43 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/15 16:31:40 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"

/**
 * Validates command binary and handles error reporting
 *
 * @param ctx Context containing shell state
 * @return int 0 if valid, error code otherwise
 */
static int	validate_child_command(t_ctx *ctx)
{
	if (!command_bin(ctx))
	{
		if (ctx->cmd->args && ctx->cmd->args[0])
		{
			if (ctx->cmd->args[0][0] == '.' && !ft_strchr(ctx->cmd->args[0],
					'/'))
				return (error_code(ERR_NO_FILE));
			if (ft_strchr(ctx->cmd->args[0], '/'))
				return (error_code(ERR_IS_DIR));
			return (error(ctx->cmd->args[0], "minishell", ERR_CMD_NOT_FOUND),
				error_code(ERR_CMD_NOT_FOUND));
		}
		return (error_code(ERR_CMD_NOT_FOUND));
	}
	return (0);
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

/**
 * Handles cleanup and exit for child process on error
 *
 * @param ctx Context containing shell state
 * @param status Exit status to use
 */
static void	child_exit_with_error(t_ctx *ctx, int status)
{
	child_process_cleanup(ctx);
	exit(status);
}

/**
 * Prepares and executes command with environment variables
 *
 * @param ctx Context containing shell state
 */
static void	run_command(t_ctx *ctx)
{
	char	**env_array;

	env_array = create_env_array(ctx->env_list);
	if (!env_array)
		child_exit_with_error(ctx, EXIT_FAILURE);
	execve(ctx->cmd->args[0], ctx->cmd->args, env_array);
	free_2d_array((void **)env_array);
	perror("execve");
	child_exit_with_error(ctx, EXIT_FAILURE);
}

/**
 * Main child process execution function
 *
 * @param ctx Context containing shell state
 */
void	execute_child(t_ctx *ctx)
{
	int	status;

	reset_signals();
	status = handle_child_redirections(ctx);
	if (status != 0)
		child_exit_with_error(ctx, status);
	status = validate_child_command(ctx);
	if (status != 0)
		child_exit_with_error(ctx, status);
	run_command(ctx);
}
