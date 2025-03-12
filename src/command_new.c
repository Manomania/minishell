/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_new.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 13:48:09 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/12 17:16:47 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Creates a new command structure
 *
 * @return t_command* New initialized command or NULL if allocation fails
 */
t_command	*command_new(void)
{
	t_command	*cmd;

	cmd = (t_command *)malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cmd->args = ft_calloc(2, sizeof(char *));
	cmd->arg_count = 0;
	cmd->redirection = NULL;
	cmd->next = NULL;
	return (cmd);
}
