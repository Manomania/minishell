/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_bin.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 18:52:18 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/13 12:27:04 by elagouch         ###   ########.fr       */
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
	if (ft_strncmp(bin_og, "cat", __INT_MAX__) == 0 || ft_strncmp(bin_og, "ls",
			__INT_MAX__) == 0 || ft_strncmp(bin_og, "grep", __INT_MAX__) == 0
		|| ft_strncmp(bin_og, "echo", __INT_MAX__) == 0 || ft_strncmp(bin_og,
			"wc", __INT_MAX__) == 0)
	{
		bin = ft_strjoin("/bin/", bin_og);
		if (!bin)
			return (false);
		if (access(bin, X_OK) != 0)
		{
			free(bin);
			return (false);
		}
		free(bin_og);
		ctx->cmd->args[0] = bin;
		return (true);
	}
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
