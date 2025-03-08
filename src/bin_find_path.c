/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bin_find_path.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 14:59:39 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/08 18:23:41 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Checks if a binary is found in a directory and is executable
 *
 * @param ctx Context for error handling
 * @param dir The path to search in
 * @param bin The binary to search for
 * @return char* Full path to binary if found and executable, NULL otherwise
 */
char	*bin_find_path(t_ctx *ctx, char *dir, char *bin)
{
	char	*tmp;
	char	*full_path;

	if (!dir || !bin)
		return (NULL);
	tmp = ft_strjoin(dir, "/");
	if (!tmp)
		ctx_error_exit(ctx, ERR_ALLOC);
	full_path = ft_strjoin(tmp, bin);
	free(tmp);
	if (!full_path)
		ctx_error_exit(ctx, ERR_ALLOC);
	if (access(full_path, X_OK) == 0)
		return (full_path);
	free(full_path);
	return (NULL);
}
