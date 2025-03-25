/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bin_find_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 15:34:50 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/24 15:35:46 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"

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
		error_print(ERROR, bin, "No such file or directory");
	else if (error_state == PATH_ERR_NO_PERMISSION)
		error_print(ERROR, bin, "Permission denied");
}
