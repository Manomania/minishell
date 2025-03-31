/* ************************************************************************** */
/*                                                                            */
/*                                                        :::     ::::::::   */
/*   env_quotes.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 17:55:13 by maximart          #+#    #+#             */
/*   Updated: 2025/03/24 11:32:12 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Handles expanding variables during string processing
 *
 * @param ctx Context containing environment information
 * @param str Input string
 * @param i Position in string (will be updated)
 * @param result Current result string
 * @return Updated result string
 */
static char	*handle_var_expansion(t_ctx *ctx, char *str, int *i, char *result)
{
	char	*var_value;

	var_value = expand_variable(ctx, str, i);
	if (!var_value)
		return (result);
	result = join_and_free(result, var_value);
	free(var_value);
	return (result);
}

/**
 * @brief Handles variable expansion during string processing
 *
 * @param ctx Context containing variable information
 * @param str String to process
 * @param i Current index pointer (modified in function)
 * @param result Current result string
 * @return Updated result string or NULL on error
 */
static char	*process_variables(t_ctx *ctx, char *str, int *i, char *result)
{
	char	*temp_result;
	int		start;

	start = *i;
	temp_result = append_part(result, str, start, *i);
	if (!temp_result)
		return (NULL);
	result = temp_result;
	result = handle_var_expansion(ctx, str, i, result);
	if (!result)
		return (NULL);
	return (result);
}

/**
 * @brief Processes a string, handling quotes and variable expansion
 *
 * @param ctx Context containing variable information
 * @param str String to process
 * @param result Current result string
 * @return Updated result string
 */
static char	*process_string(t_ctx *ctx, char *str, char *result)
{
	int		i;
	int		start;
	char	*temp_result;

	i = 0;
	start = 0;
	while (str[i])
	{
		if (str[i] == '$' && ctx->quote.in_single_quote == 1 && ctx->quote.in_double_quote == 0)
		{
			i++;
			continue ;
		}
		if (str[i] == '$')
		{
			result = process_variables(ctx, str, &i, result);
			if (!result)
				return (NULL);
			start = i;
			continue ;
		}
		i++;
	}
	temp_result = append_part(result, str, start, i);
	return (temp_result);
}


/**
 * @brief Handles quotes and variables in a string
 *
 * @param ctx Context containing variable information
 * @param str Input string to process
 * @return Newly allocated string with processed content
 */
char	*handle_quotes_and_vars(t_ctx *ctx, char *str)
{
	char	*result;
	char	*processed;

	if (!str)
		return (ft_strdup(""));
	result = ft_strdup("");
	if (!result)
		return (NULL);
	processed = process_string(ctx, str, result);
	if (!processed)
	{
		free(result);
		return (ft_strdup(""));
	}
	return (processed);
}
