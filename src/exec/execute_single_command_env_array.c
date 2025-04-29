/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_single_command_env_array.c                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 15:16:06 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/29 15:21:33 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Counts the number of environment variables in the list
 *
 * @param env_list Environment variable list
 * @return int Number of environment variables
 */
static int	escev_count_env_vars(t_env *env_list)
{
	t_env	*current;
	int		count;

	count = 0;
	current = env_list;
	while (current)
	{
		count++;
		current = current->next;
	}
	return (count);
}

/**
 * @brief Creates a formatted environment string (KEY=VALUE or KEY)
 *
 * @param key Environment variable key
 * @param value Environment variable value (can be NULL)
 * @return char* Formatted string or NULL on error
 */
static char	*escev_create_env_string(char *key, char *value)
{
	char	*key_equals;
	char	*result;

	if (value)
	{
		key_equals = ft_strjoin(key, "=");
		if (!key_equals)
			return (NULL);
		result = ft_strjoin(key_equals, value);
		free(key_equals);
		return (result);
	}
	return (ft_strdup(key));
}

/**
 * @brief Fills the environment array with formatted strings
 *
 * @param env_list Environment variable list
 * @param env_array Array to fill
 * @return int 0 on success, -1 on error
 */
static int	escev_fill_env_array(t_env *env_list, char **env_array)
{
	t_env	*current;
	int		i;

	i = 0;
	current = env_list;
	while (current)
	{
		env_array[i] = escev_create_env_string(current->key, current->value);
		if (!env_array[i])
			return (-1);
		i++;
		current = current->next;
	}
	env_array[i] = NULL;
	return (0);
}

/**
 * @brief Creates environment array from environment list
 *
 * @param env_list Environment variable list
 * @return char** New environment array or NULL on error
 */
char	**create_env_array(t_env *env_list)
{
	char	**env_array;
	int		count;
	int		result;

	count = escev_count_env_vars(env_list);
	env_array = malloc(sizeof(char *) * (count + 1));
	if (!env_array)
		return (NULL);
	result = escev_fill_env_array(env_list, env_array);
	if (result < 0)
	{
		free_2d_array((void **)env_array);
		return (NULL);
	}
	return (env_array);
}
