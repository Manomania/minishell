/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_new.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 13:51:47 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/08 13:52:19 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Creates a new pipeline structure
 *
 * @return t_pipeline* New initialized pipeline or NULL if allocation fails
 */
t_pipeline	*create_pipeline(void)
{
	t_pipeline	*pipeline;

	pipeline = (t_pipeline *)malloc(sizeof(t_pipeline));
	if (!pipeline)
		return (NULL);
	pipeline->commands = NULL;
	pipeline->next = NULL;
	pipeline->prev = NULL;
	pipeline->operator = TOK_EOF;
	return (pipeline);
}
