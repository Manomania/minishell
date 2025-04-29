/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bin_find.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 14:56:48 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/29 17:47:12 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"
#include <sys/stat.h>

/**
 * @brief Checks if a string contains path components
 *
 * Determines if the given string contains directory path components such as
 * slashes or relative path indicators ('.' or '..').
 *
 * @param str String to check for path components
 * @return true if the string contains path components, false otherwise
 */
t_bool	is_path(const char *str)
{
	int	i;

	i = 0;
	if (!str)
		return (false);
	if (str[0] == '/' || str[0] == '~' || str[0] == '.')
		return (true);
	while (str[i])
	{
		if (str[i] == '/')
			return (true);
		i++;
	}
	return (false);
}

/**
 * @brief Validates and resolves an absolute or relative path
 *
 * Checks if the path exists, has proper permissions, and is not a directory.
 * Returns appropriate error status if validation fails.
 *
 * @param bin Path to validate
 * @param error_state Pointer to store error state
 * @return Duplicated path if valid, NULL otherwise
 */
static char	*validate_path(char *bin, t_path_error *error_state)
{
	if (access(bin, F_OK) != 0)
	{
		*error_state = PATH_ERR_NOT_FOUND;
		return (NULL);
	}
	if (is_directory(bin))
	{
		*error_state = PATH_ERR_IS_DIR;
		return (NULL);
	}
	if (access(bin, X_OK) != 0)
	{
		*error_state = PATH_ERR_NO_PERMISSION;
		return (NULL);
	}
	return (ft_strdup(bin));
}

/**
 * @brief Handles path-specific error messages and returns appropriate code
 *
 * Processes path errors and displays appropriate error message based on
 * the error type (file not found, permission denied, is directory).
 *
 * @param bin The path that caused the error
 * @param error_type Type of error encountered
 * @return Error code to be used as exit status
 */
static int	bf_handle_path_error(char *bin, t_path_error error_type)
{
	if (error_type == PATH_ERR_NOT_FOUND)
		return (error(bin, NULL, ERR_NO_FILE));
	else if (error_type == PATH_ERR_NO_PERMISSION)
		return (error(bin, NULL, ERR_NO_PERMS));
	else if (error_type == PATH_ERR_IS_DIR)
		return (error(bin, NULL, ERR_IS_DIR));
	return (1);
}

/**
 * @brief Resolves a command path and handles errors appropriately
 *
 * Checks if the command is a path or a simple command and resolves it.
 * For paths, validates directly; for simple commands, searches in PATH.
 *
 * @param ctx Shell context
 * @param bin Command to resolve
 * @return Resolved path or NULL (with appropriate error displayed)
 */
char	*bin_find(t_ctx *ctx, char *bin)
{
	char			*path;
	t_path_error	error_state;

	error_state = PATH_ERR_NONE;
	if (!bin)
		return (NULL);
	if (is_path(bin))
	{
		path = validate_path(bin, &error_state);
		if (!path)
		{
			bf_handle_path_error(bin, error_state);
			return (NULL);
		}
		return (path);
	}
	path = env_find_bin(ctx, bin);
	if (!path)
		(void)error(bin, NULL, ERR_CMD_NOT_FOUND);
	return (path);
}
