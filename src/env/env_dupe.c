/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_dupe.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 17:09:18 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/16 17:11:22 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Creates a new environment node with provided key and value.
 *
 * @param key The key string to duplicate.
 * @param value The value string to duplicate, can be NULL.
 * @return A pointer to the newly created node, or NULL if allocation fails.
 */
static t_env	*create_env_node(char *key, char *value)
{
	t_env	*new_node;

	new_node = (t_env *)malloc(sizeof(t_env));
	if (!new_node)
		return (NULL);
	new_node->key = ft_strdup(key);
	if (!new_node->key)
	{
		free(new_node);
		return (NULL);
	}
	new_node->next = NULL;
	if (value)
	{
		new_node->value = ft_strdup(value);
		if (!new_node->value)
		{
			free(new_node->key);
			free(new_node);
			return (NULL);
		}
	}
	else
		new_node->value = NULL;
	return (new_node);
}

/**
 * @brief Adds a new node to the environment list.
 *
 * @param head Pointer to the head of the list.
 * @param tail Pointer to the tail of the list.
 * @param new_node The node to add to the list.
 */
static void	add_env_node(t_env **head, t_env **tail, t_env *new_node)
{
	if (!*head)
	{
		*head = new_node;
		*tail = new_node;
	}
	else
	{
		(*tail)->next = new_node;
		*tail = new_node;
	}
}

/**
 * @brief Duplicates an environment linked list.
 * Creates a deep copy of the list, including keys and values.
 *
 * @param original_list The head of the t_env list to duplicate.
 * @return A pointer to the head of the newly allocated duplicate list,
 * or NULL if memory allocation fails at any point.
 * @note The caller is responsible for freeing the returned list using
 * free_env_list.
 */
t_env	*duplicate_env_list(t_env *original_list)
{
	t_env	*new_head;
	t_env	*current_tail;
	t_env	*current_orig;
	t_env	*new_node;

	new_head = NULL;
	current_tail = NULL;
	current_orig = original_list;
	while (current_orig)
	{
		new_node = create_env_node(current_orig->key, current_orig->value);
		if (!new_node)
		{
			free_env_list(new_head);
			return (NULL);
		}
		add_env_node(&new_head, &current_tail, new_node);
		current_orig = current_orig->next;
	}
	return (new_head);
}
