/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bin_find.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 14:56:48 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/29 17:03:26 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"
#include <sys/stat.h>

/**
 * @brief Determines if a string is a path rather than a simple command
 *
 * Checks if the string contains path components like '/', './', '../', etc.
 *
 * @param str The string to check
 * @return t_bool true if it's a path, false if it's a simple command
 */
t_bool	is_path(const char *str)
{
	if (!str)
		return (false);
	if (str[0] == '/' || str[0] == '.')
		return (true);
	while (*str)
	{
		if (*str == '/')
			return (true);
		str++;
	}
	return (false);
}

/**
 * @brief Resolves a relative path to an absolute one using current directory
 *
 * @param bin The path to resolve
 * @return char* New allocated absolute path or NULL on error
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
 * @brief Handles the case when a command is specified as a path
 *
 * Checks existence, permission, and whether it's a directory
 *
 * @param bin The command path to check
 * @return char* Path if valid and executable, NULL otherwise with error display
 */
static char	*handle_command_path(char *bin)
{
	struct stat	file_stat;

	if (access(bin, F_OK) != 0)
	{
		error(bin, NULL, ERR_NO_FILE);
		return (NULL);
	}
	if (stat(bin, &file_stat) == 0 && S_ISDIR(file_stat.st_mode))
	{
		error(bin, NULL, ERR_IS_DIR);
		return (NULL);
	}
	if (access(bin, X_OK) != 0)
	{
		error(bin, NULL, ERR_NO_PERMS);
		return (NULL);
	}
	return (ft_strdup(bin));
}

/**
 * @brief Finds a binary in the PATH or as a direct path
 *
 * Tries to find the binary in PATH if it's a simple command,
 * or validates it as a direct path if specified as such.
 *
 * @param ctx Context
 * @param bin The binary name or path to find
 * @return char* Path to binary if found, NULL otherwise (with proper error)
 */
char	*bin_find(t_ctx *ctx, char *bin)
{
	char	*path;

	if (!bin)
		return (NULL);
	if (is_path(bin))
		return (handle_command_path(bin));
	path = env_find_bin(ctx, bin);
	if (!path)
		error(bin, NULL, ERR_CMD_NOT_FOUND);
	return (path);
}
