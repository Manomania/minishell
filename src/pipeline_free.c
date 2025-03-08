/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_free.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 17:44:40 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/08 17:44:55 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Frees a pipeline structure and all its resources
 *
 * @param pipeline Pipeline to free
 */
void	pipeline_free(t_pipeline *pipeline)
{
	t_command	*cmd;
	t_command	*next_cmd;

	if (!pipeline)
		return ;
	cmd = pipeline->commands;
	while (cmd)
	{
		next_cmd = cmd->next;
		command_free(cmd);
		cmd = next_cmd;
	}
	free(pipeline);
}
