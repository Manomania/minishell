/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bin_find.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 14:56:48 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/08 15:04:13 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Checks if a string is a path and not just a command
 *
 * This function determines whether the input string represents a path
 * by checking for directory-related characters like slashes and dots.
 *
 * @param str The string to check
 * @return t_bool true if the string is a path, false if it's just a command
 *
 * @example
 * is_path("ls")        // Returns false (command)
 * is_path("./ls")      // Returns true (relative path)
 * is_path("../bin/ls") // Returns true (relative path with parent dir)
 * is_path("/bin/ls")   // Returns true (absolute path)
 * is_path("~/bin/ls")  // Returns true (home directory path)
 * is_path(NULL)        // Returns false (null pointer protection)
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
 * @brief Finds a binary in the PATH variable or in the current directory
 *
 * @param bin The binary to search for
 * @return A full path to the binary if it is found and executable, or NULL.
 */
char	*bin_find(t_ctx *ctx, char *bin)
{
	if (!bin)
		return (NULL);
	if (is_path(bin))
		return (bin_find_path(ctx, (char *)"./", bin));
	return (env_find_bin(ctx, bin));
}
