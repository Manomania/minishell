/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_quotes.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maximart <maximart@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 17:55:13 by maximart          #+#    #+#             */
/*   Updated: 2025/03/18 17:57:02 by maximart         ###   ########.fr       */
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
	char	*var_value;

	i = 0;
	start = 0;
	in_squote = 0;
	in_dquote = 0;
	while (str[i])
	{
		update_quote_state(str[i], &in_squote, &in_dquote);
		if (str[i] == '$' && !in_squote)
		{
			result = append_part(result, str, start, i);
			var_value = expand_variable(ctx, str, &i, in_squote);
			result = join_and_free(result, var_value);
			free(var_value);
			start = i;
			continue ;
		}
		i++;
	}
	return (append_part(result, str, start, i));
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

	if (!str)
		return (ft_strdup(""));
	result = ft_strdup("");
	result = process_string(ctx, str, result);
	return (result);
}
