/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_expand.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 15:09:18 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/24 18:19:36 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Finds the end of a variable name
 *
 * @param str String containing the variable
 * @param start Start position of the variable (after $)
 * @return End position of the variable name
 */
static int	find_var_end(char *str, int start)
{
	int	end;

	end = start + 1;
	while (str[end] && (ft_isalnum(str[end]) || str[end] == '_'))
		end++;
	return (end);
}

/**
 * @brief Processes a single environment variable in a string
 *
 * @param ctx Context containing environment info
 * @param result String being processed
 * @param var_start Pointer to the start of the variable ($ character)
 * @return Updated string with variable expanded or NULL on error
 */
static char	*process_one_variable(t_ctx *ctx, char *result, char *var_start)
{
	int		i;
	int		j;
	char	*var_name;
	char	*var_value;
	char	*new_result;

	i = var_start - result;
	j = find_var_end(result, i);
	var_name = extract_var_name(result, i, j);
	if (!var_name)
	{
		free(result);
		return (NULL);
	}
	var_value = get_env_value(ctx->env_list, var_name);
	if (!var_value)
	{
		free(result);
		return (NULL);
	}
	new_result = replace_substring(result, i, j, var_value);
	free(var_value);
	return (new_result);
}

/**
 * @brief Expands environment variables in a line
 *
 * @param ctx Context containing environment info
 * @param line Line to expand variables in
 * @return Newly allocated string with expanded variables
 */
char	*expand_variables_in_line(t_ctx *ctx, char *line)
{
	char	*result;
	char	*var_start;

	result = init_expansion(line);
	if (!result)
		return (NULL);
	var_start = ft_strchr(result, '$');
	while (var_start)
	{
		result = process_one_variable(ctx, result, var_start);
		if (!result)
			return (NULL);
		var_start = ft_strchr(result, '$');
	}
	return (result);
}
