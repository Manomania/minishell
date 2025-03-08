/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_find_bin.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 14:40:56 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/08 15:07:57 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	**env_get_path_dirs(t_ctx *ctx)
{
	char	*path_var;
	char	**path_dirs;

	path_var = env_find(ctx, (char *)"PATH=");
	if (!path_var)
		return (NULL);
	path_dirs = ft_split(path_var, ':');
	free(path_var);
	if (!path_dirs)
		ctx_error_exit(ctx, ERR_ALLOC);
	return (path_dirs);
}

/**
 * @brief Tries to find a binary
 *
 * @param ctx Context
 * @param bin The binary to search for
 * @return A full path to the binary if it is found and executable, or NULL.
 *
 * @exception	ENOMEM if malloc fails
 */
char	*env_find_bin(t_ctx *ctx, char *bin)
{
	char	**path_dirs;
	char	**og_path_dirs;
	char	*path;

	if (!bin)
		return (NULL);
	path_dirs = env_get_path_dirs(ctx);
	og_path_dirs = path_dirs;
	while (*path_dirs)
	{
		path = bin_find_path(ctx, *path_dirs, bin);
		if (path)
			return (free_2d_array((void **)og_path_dirs), path);
		path_dirs++;
	}
	free_2d_array((void **)og_path_dirs);
	return (NULL);
}
