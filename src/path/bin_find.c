/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bin_find.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 14:56:48 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/26 12:29:24 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"

/**
 * @brief Checks if a string is a path and not just a command
 *
 * @param str The string to check
 * @return t_bool true if the string is a path, false if just a command
 */
static t_bool	is_path(const char *str)
{
	int	i;

	i = 0;
	if (!str)
		return (false);
	if (str[0] == '~' && (str[1] == '/' || str[1] == '\0'))
		return (true);
	if (str[0] == '/')
		return (true);
	while (str[i])
	{
		if (str[i] == '/' || (str[i] == '.' && (str[i + 1] == '/' || str[i
					+ 1] == '\0' || (str[i + 1] == '.' && (str[i + 2] == '/'
							|| str[i + 2] == '\0')))))
			return (true);
		i++;
	}
	return (false);
}

/**
 * @brief Tries to execute file directly if it's a path
 *
 * @param bin Binary path to check
 * @param error_state Pointer to error state variable
 * @return char* strdup of bin if executable, NULL otherwise
 */
static char	*try_direct_path(char *bin, t_path_error *error_state)
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
 * @brief Resolves a relative path to an absolute path
 *
 * @param bin Path to resolve
 * @return char* Absolute path or NULL on error
 */
char	*resolve_relative_path(char *bin)
{
	char	*cwd;
	char	*absolute_path;
	char	*tmp;

	if (bin[0] == '/')
		return (ft_strdup(bin));
	cwd = getcwd(NULL, 0);
	if (!cwd)
		return (NULL);
	tmp = ft_strjoin(cwd, "/");
	free(cwd);
	if (!tmp)
		return (NULL);
	absolute_path = ft_strjoin(tmp, bin);
	free(tmp);
	return (absolute_path);
}

/**
 * @brief Handles the case when the binary is a path
 *
 * @param bin Binary path to check
 * @param error_state Pointer to error state variable
 * @return char* Path to binary if found, NULL otherwise
 */
static char	*handle_bin_as_path(char *bin, t_path_error *error_state)
{
	char	*path;

	path = try_direct_path(bin, error_state);
	if (path)
		return (path);
	if (*error_state == PATH_ERR_IS_DIR)
		return (display_path_error(bin, *error_state), NULL);
	path = check_relative_path(bin, error_state);
	if (path)
		return (path);
	path = bin_find_path(".", bin);
	if (!path)
		display_path_error(bin, *error_state);
	return (path);
}

/**
 * @brief Finds a binary in the PATH or current directory
 *
 * @param ctx Context
 * @param bin The binary to search for
 * @return char* Path to binary if found, NULL otherwise
 */
char	*bin_find(t_ctx *ctx, char *bin)
{
	char			*path;
	t_path_error	error_state;

	error_state = PATH_ERR_NONE;
	if (!bin)
		return (NULL);
	if (is_path(bin))
		return (handle_bin_as_path(bin, &error_state));
	else
	{
		path = env_find_bin(ctx, bin);
		if (!path)
			error_print(ERROR, bin, "Command not found");
	}
	return (path);
}
