/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_bin.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 18:52:18 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/29 18:53:19 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Finds the binary path and updates the command arg[0].
 * Sets ctx->exit_status if bin_find fails.
 *
 * @param ctx Context (contains command and allows setting exit status)
 * @return t_bool true if binary found and updated,
	false otherwise (error occurred).
 */
t_bool	command_bin(t_ctx *ctx)
{
	char	*bin_og;
	char	*bin_path;

	if (!ctx || !ctx->cmd || !ctx->cmd->args || !ctx->cmd->args[0])
		return (false);
	bin_og = ctx->cmd->args[0];
	ctx->exit_status = 0;
	bin_path = bin_find(ctx, bin_og);
	if (!bin_path)
		return (false);
	free(bin_og);
	ctx->cmd->args[0] = bin_path;
	return (true);
}
