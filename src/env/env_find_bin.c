/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_find_bin.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 14:40:56 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/13 12:35:53 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Creates default path directories when PATH env var is not set
 *
 * @return char** Array of default path directories or NULL if error
 */
static char	**create_default_path_dirs(void)
{
	char	**path_dirs;

	path_dirs = malloc(sizeof(char *) * 5);
	if (!path_dirs)
		return (NULL);
	path_dirs[0] = ft_strdup("/bin");
	path_dirs[1] = ft_strdup("/usr/bin");
	path_dirs[2] = ft_strdup("/usr/local/bin");
	path_dirs[3] = ft_strdup("/sbin");
	path_dirs[4] = NULL;
	return (path_dirs);
}

/**
 * @brief Normalizes path directories by removing trailing slashes
 *
 * @param path_dirs Array of path directories to normalize
 */
static void	normalize_path_dirs(char **path_dirs)
{
	int	i;
	int	len;

	i = 0;
	while (path_dirs[i])
	{
		len = ft_strlen(path_dirs[i]);
		if (len > 0 && path_dirs[i][len - 1] == '/')
			path_dirs[i][len - 1] = '\0';
		i++;
	}
}

/**
 * @brief Gets PATH directories from environment
 *
 * @param ctx Context
 * @return char** Array of path directories or NULL if error
 */
static char	**env_get_path_dirs(t_ctx *ctx)
{
	char	*path_var;
	char	**path_dirs;

	path_var = env_find(ctx, (char *)"PATH=");
	if (!path_var)
		return (create_default_path_dirs());
	path_dirs = ft_split(path_var, ':');
	free(path_var);
	if (!path_dirs)
	{
		ctx_error(ERR_ALLOC);
		return (NULL);
	}
	normalize_path_dirs(path_dirs);
	return (path_dirs);
}

/**
 * @brief Tries to find a binary in PATH
 *
 * @param ctx Context
 * @param bin The binary to search for
 * @return char* A full path to the binary if found and executable, or NULL
 */
char	*env_find_bin(t_ctx *ctx, char *bin)
{
	char	**path_dirs;
	char	**og_path_dirs;
	char	*path;

	if (!bin)
		return (NULL);
	path_dirs = env_get_path_dirs(ctx);
	if (!path_dirs)
		return (NULL);
	og_path_dirs = path_dirs;
	while (*path_dirs)
	{
		path = bin_find_path(ctx, *path_dirs, bin);
		if (path)
		{
			free_2d_array((void **)og_path_dirs);
			return (path);
		}
		path_dirs++;
	}
	free_2d_array((void **)og_path_dirs);
	return (NULL);
}
