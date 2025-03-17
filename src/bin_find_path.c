/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bin_find_path.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 14:59:39 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/08 14:53:46 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Checks if a binary is found in a directory and is executable and returns the
 * allocated full path if it is, or NULL if it isn't
 *
 * @param dir The path to search in
 * @param bin The binary to search for
 * @return char* A full path to the binary if it is found and executable,
 * or NULL.
 */
char	*bin_find_path(t_ctx *ctx, char *dir, char *bin)
{
	char	*tmp;
	char	*full_path;

	if (!dir || !bin)
		return (NULL);
	tmp = ft_strjoin(dir, "/");
	if (!tmp)
		ctx_error_exit(ctx, ENOMEM);
	full_path = ft_strjoin(tmp, bin);
	free(tmp);
	if (!full_path)
		ctx_error_exit(ctx, ENOMEM);
	if (access(full_path, X_OK) == 0)
		return (full_path);
	free(full_path);
	return (NULL);
}
