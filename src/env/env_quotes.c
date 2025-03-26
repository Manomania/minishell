/* ************************************************************************** */
/*                                                                            */
/*                                                        :::     ::::::::   */
/*   env_quotes.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 17:55:13 by maximart          #+#    #+#             */
/*   Updated: 2025/03/19 18:21:27 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Checks if a character is a single quote
 *
 * @param c Character to check
 * @return 1 if single quote, 0 otherwise
 */
static int	is_single_quote(char c)
{
	return (c == '\'');
}

/**
 * @brief Checks if a character is a double quote
 *
 * @param c Character to check
 * @return 1 if double quote, 0 otherwise
 */
static int	is_double_quote(char c)
{
	return (c == '"');
}

/**
 * @brief Updates quote state based on current character
 *
 * @param c Current character
 * @param in_squote Pointer to single quote state
 * @param in_dquote Pointer to double quote state
 */
static void	update_quote_state(char c, int *in_squote, int *in_dquote)
{
	if (is_single_quote(c) && !(*in_dquote))
		*in_squote = !(*in_squote);
	else if (is_double_quote(c) && !(*in_squote))
		*in_dquote = !(*in_dquote);
}

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
	char	*temp_result;
	int		in_squote;

	in_squote = 0;
	var_value = expand_variable(ctx, str, i, in_squote);
	if (!var_value)
		return (result);
	temp_result = join_and_free(result, var_value);
	free(var_value);
	return (temp_result);
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
	int		in_squote;
	int		in_dquote;
	char	*temp_result;

	i = 0;
	start = 0;
	in_squote = 0;
	in_dquote = 0;
	while (str[i])
	{
		update_quote_state(str[i], &in_squote, &in_dquote);
		if (str[i] == '$' && !in_squote)
		{
			temp_result = append_part(result, str, start, i);
			if (!temp_result)
				return (result);
			result = temp_result;
			result = handle_var_expansion(ctx, str, &i, result);
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
	return (processed);
}
