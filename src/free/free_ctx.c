/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_ctx.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 18:24:28 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/07 19:32:44 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Clears readline history and resets the readline state
 *
 * This function cleans up readline resources to prevent memory leaks.
 */
static void	cleanup_readline(void)
{
	clear_history();
	rl_free_line_state();
	rl_cleanup_after_signal();
}

/**
 * @brief Closes any open file descriptors in the context
 *
 * @param ctx Context containing file descriptors
 */
static void	close_file_descriptors(t_ctx *ctx)
{
	if (ctx->fd_file_in != -1)
	{
		close(ctx->fd_file_in);
		ctx->fd_file_in = -1;
	}
	if (ctx->fd_file_out != -1)
	{
		close(ctx->fd_file_out);
		ctx->fd_file_out = -1;
	}
}

/**
 * @brief Frees environment, tokens and command structures from context
 *
 * @param ctx Context to clean up
 */
static void	free_structures(t_ctx *ctx)
{
	if (ctx->tokens)
	{
		free_all_token(ctx->tokens);
		ctx->tokens = NULL;
	}
	if (ctx->cmd)
	{
		free_all_commands(ctx->cmd);
		ctx->cmd = NULL;
	}
	if (ctx->env_list)
	{
		free_env_list(ctx->env_list);
		ctx->env_list = NULL;
	}
}

/**
 * @brief Clears the context and frees all allocated resources
 *
 * @param ctx context
 */
void	ctx_clear(t_ctx *ctx)
{
	if (!ctx)
		return ;
	free_structures(ctx);
	cleanup_readline();
	close_file_descriptors(ctx);
	free(ctx);
}

/**
 * @brief Cleanup function to ensure all resources are freed at exit
 *
 * @param ctx Shell context
 * @return void
 */
void	final_cleanup(t_ctx *ctx)
{
	if (ctx->tokens)
	{
		free_all_token(ctx->tokens);
		ctx->tokens = NULL;
	}
	if (ctx->cmd)
	{
		free_all_commands(ctx->cmd);
		ctx->cmd = NULL;
	}
	if (ctx->env_list)
	{
		free_env_list(ctx->env_list);
		ctx->env_list = NULL;
	}
	free(ctx);
}
