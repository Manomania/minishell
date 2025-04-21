/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 16:04:32 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/21 16:05:44 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Handles positional parameter expansion
 *
 * @param str Input string
 * @param i Current position (will be updated)
 * @return char* Empty string for unsupported positional parameters
 */
char	*expand_positional_param(char *str, int *i)
{
	if (str[*i] == '@' || str[*i] == '!' || str[*i] == '&' || (str[*i] >= '1'
			&& str[*i] <= '9'))
	{
		(*i)++;
		return (ft_strdup(""));
	}
	return (NULL);
}

/**
 * @brief Handles standard environment variable expansion
 *
 * @param ctx Shell context
 * @param str Input string
 * @param i Current position (will be updated)
 * @return char* Expanded environment variable or empty string
 */
char	*expand_env_var(t_ctx *ctx, char *str, int *i)
{
	char	*var_name;
	char	*var_value;
	char	*full_varname;

	var_name = get_var_name(str, i);
	if (!var_name)
		return (ft_strdup("$"));
	full_varname = ft_strjoin(var_name, "=");
	var_value = expand_var(ctx, var_name);
	free(var_name);
	free(full_varname);
	if (!var_value)
		return (ft_strdup(""));
	return (var_value);
}

/**
 * @brief Handles variable expansion based on quote context
 *
 * @param ctx Shell context
 * @param str Input string
 * @param i Current position (will be updated)
 * @return Expanded variable or dollar sign
 */
char	*expand_variable(t_ctx *ctx, char *str, int *i)
{
	char	*result;

	(*i)++;
	result = expand_special_var(ctx, str, i);
	if (result)
		return (result);
	result = expand_positional_param(str, i);
	if (result)
		return (result);
	(*i)--;
	return (expand_env_var(ctx, str, i));
}
