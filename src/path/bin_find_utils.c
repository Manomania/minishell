/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bin_find_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 15:34:50 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/28 11:25:30 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"
#include <sys/stat.h>

/**
 * @brief Checks if a path is a directory
 *
 * @param path Path to check
 * @return t_bool true if it's a directory, false otherwise
 */
t_bool	is_directory(const char *path)
{
	struct stat	path_stat;

	if (stat(path, &path_stat) != 0)
		return (false);
	return (S_ISDIR(path_stat.st_mode));
}

/**
 * @brief Checks and resolves a relative path binary
 *
 * @param bin Binary name to resolve
 * @param error_state Pointer to error state variable
 * @return char* Resolved path or NULL if not found/accessible
 */
char	*check_relative_path(char *bin, t_path_error *error_state)
{
	char	*path;

	if (bin[0] == '/' || *error_state != PATH_ERR_NONE)
		return (NULL);
	path = resolve_relative_path(bin);
	if (!path)
		return (NULL);
	if (access(path, X_OK) == 0)
		return (path);
	if (access(path, F_OK) == 0)
		*error_state = PATH_ERR_NO_PERMISSION;
	else if (is_directory(path))
		*error_state = PATH_ERR_IS_DIR;
	else
		*error_state = PATH_ERR_NOT_FOUND;
	free(path);
	return (NULL);
}

/**
 * @brief Displays appropriate error message based on error state
 *
 * @param bin Binary name that caused the error
 * @param error_state Current error state
 */
void	display_path_error(char *bin, t_path_error error_state)
{
	if (error_state == PATH_ERR_NOT_FOUND)
		(void)error(bin, NULL, ERR_NO_FILE);
	else if (error_state == PATH_ERR_NO_PERMISSION)
		(void)error(bin, NULL, ERR_NO_PERMS);
	else if (error_state == PATH_ERR_IS_DIR)
		(void)error(bin, NULL, ERR_IS_DIR);
}
