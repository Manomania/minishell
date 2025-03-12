/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_free.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 13:49:39 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/12 17:17:53 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Frees all memory associated with command redirections
 *
 * @param redirections Redirections linked list
 */
static void	free_redirections(t_redirection *redirections)
{
	t_redirection	*current;
	t_redirection	*next;

	current = redirections;
	while (current)
	{
		next = current->next;
		if (current->filename)
			free(current->filename);
		free(current);
		current = next;
	}
}

/**
 * @brief Frees a command structure and all its resources
 *
 * @param cmd Command to free
 */
void	command_free(t_command *cmd)
{
	int	i;

	if (!cmd)
		return ;
	if (cmd->args[0])
	{
		free(cmd->args[0]);
		cmd->args[0] = NULL;
	}
	if (cmd->args)
	{
		i = 0;
		while (cmd->args[i] != NULL)
		{
			free(cmd->args[i]);
			i++;
		}
		free(cmd->args);
	}
	free_redirections(cmd->redirection);
	free(cmd);
}
