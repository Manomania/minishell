/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_bin.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 18:52:18 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/13 12:34:24 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Finds the binary path and updates the command arg
 *
 * @param ctx Context
 * @return t_bool true if successful, false otherwise
 */
t_bool	command_bin(t_ctx *ctx)
{
	char	*bin_og;
	char	*bin;

	if (!ctx->cmd->args || !ctx->cmd->args[0])
		return (false);
	bin_og = ctx->cmd->args[0];
	bin = bin_find(ctx, bin_og);
	if (!bin)
	{
		ft_printf("Command not found: %s\n", bin_og);
		free(bin_og);
		ctx->cmd->args[0] = NULL;
		return (false);
	}
	free(bin_og);
	ctx->cmd->args[0] = bin;
	return (true);
}
