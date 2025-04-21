/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_execute_utils4.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 16:24:04 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/15 16:26:44 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"

/**
 * @brief Counts the number of environment variables in the list
 *
 * @param env_list Environment variable list
 * @return int Number of environment variables
 */
static int	count_env_vars(t_env *env_list)
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
 * @brief Creates a single environment string in KEY=VALUE format
 *
 * @param key Environment variable key
 * @param value Environment variable value
 * @return char* Combined string or NULL on error
 */
static char	*create_env_string(char *key, char *value)
{
	char	*tmp;
	char	*result;

	tmp = ft_strjoin(key, "=");
	if (!tmp)
		return (NULL);
	if (!value)
		value = "";
	result = ft_strjoin(tmp, value);
	free(tmp);
	return (result);
}

/**
 * @brief Populates environment array with KEY=VALUE strings
 *
 * @param env_array Array to populate
 * @param env_list Environment variable list
 * @return int 0 on success, -1 on error
 */
static int	populate_env_array(char **env_array, t_env *env_list)
{
	t_env	*current;
	int		i;

	current = env_list;
	i = 0;
	while (current)
	{
		env_array[i] = create_env_string(current->key, current->value);
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
	int		status;

	count = count_env_vars(env_list);
	env_array = malloc(sizeof(char *) * (count + 1));
	if (!env_array)
		return (NULL);
	status = populate_env_array(env_array, env_list);
	if (status == -1)
	{
		free_2d_array((void **)env_array);
		return (NULL);
	}
	return (env_array);
}
