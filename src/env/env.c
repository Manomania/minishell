/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maximart <maximart@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 12:30:04 by maximart          #+#    #+#             */
/*   Updated: 2025/03/14 11:11:24 by maximart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Gets the value of an environment variable
 *
 * @param env_list Environment variable list
 * @param key Key to look for
 * @return Value of the variable or NULL if not found
 */
char	*get_env_value(t_env *env_list, char *key)
{
	t_env	*current;

	current = env_list;
	while (current)
	{
		if (ft_strncmp(current->key, key, ft_strlen(key)) == 0
			&& ft_strlen(current->key) == ft_strlen(key))
			return (current->value);
		current = current->next;
	}
	return (NULL);
}

/**
 * @brief Gets a variable name from a string
 *
 * @param str String containing the variable
 * @param pos Position in the string (will be updated)
 * @return Newly allocated string with variable name or NULL
 */
static char	*get_var_name(char *str, int *pos)
{
	char	*name;
	int		len;
	int		i;

	i = *pos;
	if (str[i] == '?')
	{
		(*pos)++;
		return (ft_strdup("?"));
	}
	len = 0;
	while (ft_isalnum(str[i + len]) || str[i + len] == '_')
		len++;
	if (len == 0)
		return (NULL);
	name = malloc(len + 1);
	if (!name)
		return (NULL);
	ft_strlcpy(name, str + i, len + 1);
	*pos += len;
	return (name);
}

/**
 * @brief Expands a single environment variable
 *
 * @param ctx Shell context
 * @param var_name Name of the variable to expand
 * @return New allocated string with variable value or empty string
 */
char	*expand_var(t_ctx *ctx, char *var_name)
{
	char	*value;

	if (!var_name)
		return (ft_strdup(""));
	if (ft_strncmp(var_name, "?", ft_strlen(var_name)) == 0)
		return (ft_itoa(ctx->exit_status));
	value = get_env_value(ctx->env_list, var_name);
	if (value)
		return (ft_strdup(value));
	return (ft_strdup(""));
}

/**
 * @brief Process a part of a string and append it to the result
 *
 * @param result Current result string
 * @param str Input string
 * @param start Start position
 * @param end End position
 * @return Updated result string
 */
static char	*append_part(char *result, char *str, int start, int end)
{
	char	*part;

	if (end <= start)
		return (result);
	part = ft_substr(str, start, end - start);
	if (!part)
		return (result);
	return (join_and_free(result, part));
}


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
 * @brief Handles variable expansion based on quote context
 *
 * @param ctx Shell context
 * @param str Input string
 * @param i Current position (will be updated)
 * @param in_squote Whether in single quotes
 * @return Expanded variable or dollar sign
 */
static char	*expand_variable(t_ctx *ctx, char *str, int *i, int in_squote)
{
	char	*var_name;
	char	*var_value;

	(*i)++;
	if (in_squote)
		return (ft_strdup("$"));
	var_name = get_var_name(str, i);
	var_value = expand_var(ctx, var_name);
	free(var_name);
	return (var_value);
}

/**
 * @brief Processes a string, handling quotes and variable expansion
 *
 * @param ctx Context containing variable information
 * @param str String to process
 * @param result Current result string
 * @return Updated result string
 */
char	*process_string(t_ctx *ctx, char *str, char *result)
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
			continue;
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