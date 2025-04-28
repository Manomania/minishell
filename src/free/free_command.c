/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_command.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 13:49:39 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/25 12:40:52 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Frees memory allocated for all redirections in a linked list
 *
 * This function traverses a linked list of redirections and frees each one,
 * including the filename string within each redirection.
 *
 * @param redirection First redirection in the linked list
 */
static void	free_all_redirections(t_redirection *redirection)
{
	t_redirection	*current;
	t_redirection	*next;

	current = redirection;
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
 * @brief Frees memory allocated for a command structure
 * Frees the arguments array, individual arguments, redirections, and the
 * command struct itself.
 *
 * @param cmd Command structure to free
 */
void	free_command(t_command *cmd)
{
	int	i;

	if (!cmd)
		return ;
	if (cmd->args)
	{
		i = 0;
		while (i <= cmd->arg_count)
		{
			if (cmd->args[i])
				free(cmd->args[i]);
			i++;
		}
		free(cmd->args);
		cmd->args = NULL;
	}
	free_all_redirections(cmd->redirection);
	cmd->redirection = NULL;
	free(cmd);
}

/**
 * @brief Frees memory allocated for all commands in a pipeline
 *
 * @param cmd First command in the linked list
 */
void	free_all_commands(t_command *cmd)
{
	t_command	*current;
	t_command	*next;

	current = cmd;
	while (current)
	{
		next = current->next;
		free_command(current);
		current = next;
	}
}
