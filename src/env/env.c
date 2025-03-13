/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maximart <maximart@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 12:30:04 by maximart          #+#    #+#             */
/*   Updated: 2025/03/13 12:31:45 by maximart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
 * @brief Appends a string to the result and frees both strings
 *
 * @param s1 First string (will be freed)
 * @param s2 Second string (will be freed)
 * @return New allocated concatenated string
 */
static char	*join_and_free(char *s1, char *s2)
{
	char	*result;

	if (!s1)
		return (s2);
	if (!s2)
		return (s1);
	result = ft_strjoin(s1, s2);
	free(s1);
	free(s2);
	return (result);
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
 * @brief Process variables in a string
 *
 * @param ctx Shell context
 * @param str Input string
 * @param i Current position (will be updated)
 * @param in_quotes Whether we're inside quotes
 * @return Processed string with variables expanded
 */
static char	*process_vars(t_ctx *ctx, char *str, int *i, int in_quotes)
{
	char	*result;
	char	*var_name;
	char	*var_value;

	result = ft_strdup("");
	(*i)++;
	if (str[*i] == '\'' && in_quotes)
	{
		result = join_and_free(result, ft_strdup("$"));
		return (result);
	}
	var_name = get_var_name(str, i);
	if (!var_name)
	{
		result = join_and_free(result, ft_strdup("$"));
		return (result);
	}
	var_value = expand_var(ctx, var_name);
	result = join_and_free(result, var_value);
	free(var_name);
	return (result);
}

/**
 * @brief Process a quoted string
 *
 * @param ctx Shell context
 * @param str Input string
 * @param i Current position (will be updated)
 * @param quote The quote character (' or ")
 * @return Processed string with quotes expanded
 */
static char	*process_quotes(t_ctx *ctx, char *str, int *i, char quote)
{
	char	*result;
	int		start;
	char	*part;

	result = ft_strdup("");
	(*i)++;
	start = *i;
	while (str[*i] && str[*i] != quote)
	{
		if ((str[*i] == '$' && quote == '"')
			|| (str[*i - 1] == '\'' && str[*i] == '$'))
		{
			result = append_part(result, str, start, *i);
			part = process_vars(ctx, str, i, 1);
			result = join_and_free(result, part);
			start = *i;
		}
		else
			(*i)++;
	}
	result = append_part(result, str, start, *i);
	if (str[*i])
		(*i)++;
	return (result);
}

/**
 * @brief Handles quotes and environment variables in a string
 *
 * @param ctx Shell context
 * @param str String to process
 * @return New string with variables expanded according to quoting rules
 */
char	*handle_quotes_and_vars(t_ctx *ctx, char *str)
{
	int		i;
	char	*result;
	char	*part;
	int		start;

	i = 0;
	result = ft_strdup("");
	start = 0;
	while (str[i])
	{
		if (str[i] == '\'' || str[i] == '"')
		{
			result = append_part(result, str, start, i);
			part = process_quotes(ctx, str, &i, str[i]);
			result = join_and_free(result, part);
			start = i;
		}
		else if (str[i] == '$')
		{
			result = append_part(result, str, start, i);
			part = process_vars(ctx, str, &i, 0);
			result = join_and_free(result, part);
			start = i;
		}
		else
			i++;
	}
	result = append_part(result, str, start, i);
	return (result);
}

/**
 * @brief Expands environment variables in a string
 *
 * @param ctx Shell context
 * @param str String containing variables to expand
 * @return New string with variables expanded
 */
char	*expand_env_vars(t_ctx *ctx, char *str)
{
	int		i;
	char	*result;
	char	*part;
	int		start;

	i = 0;
	result = ft_strdup("");
	start = 0;
	while (str[i])
	{
		if (str[i] == '$')
		{
			result = append_part(result, str, start, i);
			part = process_vars(ctx, str, &i, 0);
			result = join_and_free(result, part);
			start = i;
		}
		else
			i++;
	}
	result = append_part(result, str, start, i);
	return (result);
}