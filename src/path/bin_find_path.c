/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bin_find_path.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 14:59:39 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/17 17:46:35 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"

/**
 * @brief Checks if a file exists and is executable
 *
 * @param path Path to the file to check
 * @return int 1 if file exists and is executable, 0 otherwise
 */
static int	is_executable(const char *path)
{
	if (access(path, F_OK) == 0)
	{
		if (access(path, X_OK) == 0)
			return (1);
	}
	return (0);
}

/**
 * @brief Checks if a binary is found in a directory and is executable
 *
 * @param dir The path to search in
 * @param bin The binary to search for
 * @return char* Full path to binary if found and executable, NULL otherwise
 */
char	*bin_find_path(const char *dir, char *bin)
{
	char	*tmp;
	char	*full_path;

	if (!dir || !bin)
		return (NULL);
	tmp = ft_strjoin(dir, "/");
	if (!tmp)
	{
		ctx_error(ERR_ALLOC);
		return (NULL);
	}
	full_path = ft_strjoin(tmp, bin);
	free(tmp);
	if (!full_path)
	{
		ctx_error(ERR_ALLOC);
		return (NULL);
	}
	if (is_executable(full_path))
		return (full_path);
	free(full_path);
	return (NULL);
}
