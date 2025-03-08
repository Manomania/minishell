/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_free.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 13:49:39 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/08 13:49:55 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Frees a command structure and all its resources
 *
 * @param cmd Command to free
 */
void	command_free(t_command *cmd)
{
	t_redirection	*redir;
	t_redirection	*next_redir;
	int				i;

	if (!cmd)
		return ;
	if (cmd->cmd)
		free(cmd->cmd);
	if (cmd->args)
	{
		i = 0;
		while (i < cmd->arg_count)
			free(cmd->args[i++]);
		free(cmd->args);
	}
	redir = cmd->redirections;
	while (redir)
	{
		next_redir = redir->next;
		free(redir->filename);
		free(redir);
		redir = next_redir;
	}
	free(cmd);
}
