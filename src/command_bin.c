/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_bin.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 18:52:18 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/11 12:12:00 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Finds the binary of a command and places it in the first argument.
 *
 * @param ctx Context
 */
t_bool	command_bin(t_ctx *ctx)
{
	char	*bin_og;
	char	*bin;

	bin_og = ctx->cmd->cmd;
	bin = bin_find(ctx, bin_og);
	if (!bin)
	{
		free(bin_og);
		ctx->cmd->cmd = NULL;
		return (false);
	}
	free(bin_og);
	ctx->cmd->cmd = bin;
	return (true);
}
