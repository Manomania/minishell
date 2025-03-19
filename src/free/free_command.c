/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_command.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 13:49:39 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/17 14:04:10 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Frees memory allocated for a redirection structure
 *
 * This function releases all memory associated with a redirection structure
 * including the filename.
 *
 * @param redirection Redirection structure to free
 */
static void	free_redirection(t_redirection *redirection)
{
	if (!redirection)
		return ;
	if (redirection->filename)
		free(redirection->filename);
	free(redirection);
}

/**
 * @brief Frees memory allocated for all redirections in a linked list
 *
 * This function traverses a linked list of redirections and frees each one.
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
		free_redirection(current);
		current = next;
	}
}

/**
 * @brief Frees memory allocated for command arguments
 *
 * This function releases all memory associated with the args array.
 *
 * @param args Array of argument strings
 * @param arg_count Number of arguments in the array
 */
static void	free_command_args(char **args, int arg_count)
{
	int	i;

	if (!args)
		return ;
	i = 0;
	while (i <= arg_count)
	{
		if (args[i])
			free(args[i]);
		i++;
	}
	free(args);
}

/**
 * @brief Frees memory allocated for a command structure
 *
 * This function releases all memory associated with a command structure
 * including arguments and redirections.
 *
 * @param cmd Command structure to free
 */
void	free_command(t_command *cmd)
{
	if (!cmd)
		return ;
	free_command_args(cmd->args, cmd->arg_count);
	free_all_redirections(cmd->redirection);
	free(cmd);
}

/**
 * @brief Frees memory allocated for all commands in a pipeline
 *
 * This function traverses a linked list of commands and frees each one.
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
