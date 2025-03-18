/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maximart <maximart@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 13:43:13 by maximart          #+#    #+#             */
/*   Updated: 2025/03/10 13:43:23 by maximart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_redirection(t_redirection *redirection)
{
	if (redirection)
	{
		free(redirection->filename);
		free(redirection);
	}
}

void	free_all_redirection(t_redirection *redirection)
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

void	free_command_pipeline(t_command *cmd)
{
	t_command	*current;
	t_command	*next;
	int			i;

	current = cmd;
	while (current)
	{
		next = current->next;
		if (current->args)
		{
			i = 0;
			while (i < current->arg_count)
			{
				free(current->args[i]);
				i++;
			}
			free(current->args);
		}
		free_all_redirection(current->redirection);
		free(current);
		current = next;
	}
}

void	free_command(t_command *cmd)
{
	int	i;

	if (!cmd)
		return ;
	if (cmd->args)
	{
		i = 0;
		while (i < cmd->arg_count)
		{
			free(cmd->args[i]);
			i++;
		}
		free(cmd->args);
	}
	free_all_redirection(cmd->redirection);
	free(cmd);
}

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
