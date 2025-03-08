/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_add.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 17:47:35 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/08 17:47:52 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Adds a command to a pipeline
 *
 * @param pipeline Pipeline to add the command to
 * @param cmd Command to add
 */
void	pipeline_add_command(t_pipeline *pipeline, t_command *cmd)
{
	t_command	*current;

	if (!pipeline->commands)
	{
		pipeline->commands = cmd;
		return ;
	}
	current = pipeline->commands;
	while (current->next)
		current = current->next;
	current->next = cmd;
	cmd->prev = current;
}
