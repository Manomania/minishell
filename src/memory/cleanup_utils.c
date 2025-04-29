/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 17:36:45 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/29 17:36:48 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Safely frees a string and sets pointer to NULL
 *
 * Prevents double free issues by checking if pointer is NULL first.
 *
 * @param str Pointer to string pointer to free
 */
void	safe_free_str(char **str)
{
	if (str && *str)
	{
		free(*str);
		*str = NULL;
	}
}

/**
 * @brief Ensures all memory is properly freed before command execution
 *
 * Cleans up any transient resources before executing a command.
 *
 * @param ctx Shell context
 */
void	cleanup_before_command(t_ctx *ctx)
{
	if (ctx->tokens)
	{
		free_all_token(ctx->tokens);
		ctx->tokens = NULL;
	}
}

/**
 * @brief Properly closes any open file descriptors
 *
 * Prevents file descriptor leaks by closing any descriptors still open.
 *
 * @param ctx Shell context
 */
void	close_open_fds(t_ctx *ctx)
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
