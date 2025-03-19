/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_variable.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maximart <maximart@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/19 18:01:20 by maximart          #+#    #+#             */
/*   Updated: 2025/03/19 18:01:24 by maximart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "minishell.h"

/**
 * @brief Process variable name and get its value
 *
 * @param ctx Context containing environment information
 * @param line Input string with potential variables
 * @param i Current position in string (updated in-place)
 * @return char* Value of the variable or NULL on error
 */
static char	*process_var_name(t_ctx *ctx, char *line, int *i)
{
	int		j;
	char	*var_name;
	char	*var_value;

	j = *i;
	while (line[j] && (ft_isalnum(line[j]) || line[j] == '_'))
		j++;
	var_name = ft_substr(line, *i, j - *i);
	if (!var_name)
		return (NULL);
	var_value = env_find(ctx, var_name);
	free(var_name);
	*i = j;
	return (var_value);
}

/**
 * @brief Handle variable replacement in the result string
 *
 * @param ctx Context containing environment information
 * @param line Input string
 * @param result Current result string
 * @param i Current position (updated in-place)
 * @return char* Updated result string or NULL on error
 */
static char	*handle_variable(t_ctx *ctx, char *line, char *result, int *i)
{
	char	*var_value;
	char	*new_result;

	(*i)++;
	var_value = process_var_name(ctx, line, i);
	if (var_value)
	{
		new_result = ft_strjoin(result, var_value);
		free(var_value);
		free(result);
		return (new_result);
	}
	return (result);
}

/**
 * @brief Add normal text to the result string
 *
 * @param line Input string
 * @param result Current result string
 * @param start Start position in line
 * @param end End position in line
 * @return char* Updated result string or NULL on error
 */
static char	*add_text_segment(char *line, char *result, int start, int end)
{
	char	*segment;
	char	*new_result;

	if (start >= end)
		return (result);
	segment = ft_substr(line, start, end - start);
	if (!segment)
	{
		free(result);
		return (NULL);
	}
	new_result = ft_strjoin(result, segment);
	free(segment);
	free(result);
	return (new_result);
}

/**
 * @brief Expands environment variables in a string
 *
 * @param ctx Context containing environment information
 * @param line Input string with potential variables to expand
 * @return char* New allocated string with variables expanded
 */
char	*expand_variables(t_ctx *ctx, char *line)
{
	int		i;
	int		start;
	char	*result;

	if (!line)
		return (NULL);
	result = ft_strdup("");
	if (!result)
		return (NULL);
	i = 0;
	start = 0;
	while (line[i])
	{
		if (line[i] == '$')
		{
			if (i > start)
				result = add_text_segment(line, result, start, i);
			result = add_text_segment(line, result, start, i);
			if (!result)
				return (NULL);
			result = handle_variable(ctx, line, result, &i);
			if (!result)
				return (NULL);
			start = i;
		}
		else
			i++;
	}
	if (start < i)
		result = add_text_segment(line, result, start, i);
	return (result);
}