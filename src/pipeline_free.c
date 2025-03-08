/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_free.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 17:44:40 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/08 18:38:42 by elagouch         ###   ########.fr       */
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
	t_pipeline	*next_pipeline;

	if (!pipeline)
		return ;
	cmd = pipeline->commands;
	while (cmd)
	{
		next_cmd = cmd->next;
		command_free(cmd);
		cmd = next_cmd;
	}
	next_pipeline = pipeline->next;
	free(pipeline);
	if (next_pipeline)
		pipeline_free(next_pipeline);
}
