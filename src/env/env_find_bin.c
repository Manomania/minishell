/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_find_bin.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 14:40:56 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/29 16:59:48 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"

/**
 * @brief Gets PATH variable value from environment
 *
 * @param ctx Context containing environment variables
 * @return char* PATH value or NULL if not found/unset
 */
static char	*get_path_from_env(t_ctx *ctx)
{
	t_env	*env;

	env = ctx->env_list;
	while (env)
	{
		if (ft_strncmp(env->key, "PATH", 5) == 0)
			return (env->value);
		env = env->next;
	}
	return (NULL);
}

/**
 * @brief Normalizes path directories by removing trailing slashes
 *
 * @param path_dir Path directory string
 * @return Normalized path with no trailing slash
 */
static char	*normalize_path_dir(char *path_dir)
{
	int	len;

	len = ft_strlen(path_dir);
	if (len > 0 && path_dir[len - 1] == '/')
		path_dir[len - 1] = '\0';
	return (path_dir);
}

/**
 * @brief Searches for a binary in the PATH environment directories
 *
 * @param ctx Context containing environment
 * @param bin Binary name to search for
 * @return char* Full path to the binary if found, NULL otherwise
 */
char	*env_find_bin(t_ctx *ctx, char *bin)
{
	char	*path_var;
	char	**path_dirs;
	char	**current_dir;
	char	*bin_path;

	if (!bin)
		return (NULL);
	path_var = get_path_from_env(ctx);
	if (!path_var)
		return (NULL);
	path_dirs = ft_split(path_var, ':');
	if (!path_dirs)
		return (NULL);
	current_dir = path_dirs;
	while (*current_dir)
	{
		*current_dir = normalize_path_dir(*current_dir);
		bin_path = bin_find_path(*current_dir, bin);
		if (bin_path)
			return (free_2d_array((void **)path_dirs), bin_path);
		current_dir++;
	}
	free_2d_array((void **)path_dirs);
	return (NULL);
}
