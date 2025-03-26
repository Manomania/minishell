/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_expand_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 15:10:15 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/24 15:11:07 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Initializes the variable expansion process
 *
 * @param line Line to expand variables in
 * @return Newly allocated copy of line or NULL on error
 */
char	*init_expansion(char *line)
{
	char	*result;

	if (!line)
		return (NULL);
	result = ft_strdup(line);
	return (result);
}

/**
 * @brief Extracts a variable name from a string
 *
 * @param str String containing the variable
 * @param start Start position of the variable (after $)
 * @param end Position to stop extraction
 * @return Newly allocated variable name with = appended or NULL on error
 */
char	*extract_var_name(char *str, int start, int end)
{
	char	*var_name;
	char	*temp;

	var_name = ft_substr(str, start + 1, end - start - 1);
	if (!var_name)
		return (NULL);
	temp = ft_strjoin(var_name, "=");
	free(var_name);
	return (temp);
}
