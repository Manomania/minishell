/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bin_find_path.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 14:59:39 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/29 17:03:47 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"
#include <sys/stat.h>

/**
 * @brief Checks if a file exists and is executable
 *
 * @param path Full path to the file
 * @return t_bool true if file exists and is executable, false otherwise
 */
static t_bool	is_executable(const char *path)
{
	struct stat	path_stat;

	if (access(path, F_OK) != 0)
		return (false);
	if (stat(path, &path_stat) != 0)
		return (false);
	if (S_ISDIR(path_stat.st_mode))
		return (false);
	if (access(path, X_OK) != 0)
		return (false);
	return (true);
}

/**
 * @brief Checks for a binary in a specific directory
 *
 * @param dir Directory to search in
 * @param bin Binary name to search for
 * @return char* Full path if found and executable, NULL otherwise
 */
char	*bin_find_path(const char *dir, char *bin)
{
	char	*tmp;
	char	*full_path;

	if (!dir || !bin)
		return (NULL);
	tmp = ft_strjoin(dir, "/");
	if (!tmp)
		return (NULL);
	full_path = ft_strjoin(tmp, bin);
	free(tmp);
	if (!full_path)
		return (NULL);
	if (is_executable(full_path))
		return (full_path);
	free(full_path);
	return (NULL);
}
