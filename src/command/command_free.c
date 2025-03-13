/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_free.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 13:49:39 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/13 11:50:44 by elagouch         ###   ########.fr       */
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
 * @brief Frees all arguments in a command
 *
 * @param args Array of argument strings
 * @param count Number of arguments (excluding NULL terminator)
 */
static void	free_command_args(char **args, int count)
{
	int	i;

	if (!args)
		return ;
	i = 0;
	while (i <= count)
	{
		if (args[i])
			free(args[i]);
		i++;
	}
	free(args);
}

/**
 * @brief Frees a command structure and all its resources
 *
 * @param cmd Command to free
 */
void	command_free(t_command *cmd)
{
	if (!cmd)
		return ;
	if (cmd->args)
		free_command_args(cmd->args, cmd->arg_count);
	free_redirections(cmd->redirection);
	free(cmd);
}