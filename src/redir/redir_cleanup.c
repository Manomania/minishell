/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_cleanup.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 19:26:26 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/07 20:06:03 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Closes file descriptors related to a specific command
 *
 * @param cmd Command whose file descriptors should be closed
 */
static void	close_cmd_file_descriptors(t_command *cmd)
{
	t_redirection	*redir;

	if (!cmd)
		return ;
	redir = cmd->redirection;
	while (redir)
	{
		if (redir->fd > 2)
		{
			close(redir->fd);
			redir->fd = -1;
		}
		redir = redir->next;
	}
}

/**
 * @brief Ensures all heredoc resources are properly freed
 *
 * @param ctx Context containing tokens and commands
 * @return void
 */
void	cleanup_heredoc_resources(t_ctx *ctx)
{
	t_command	*cmd;

	if (!ctx || !ctx->cmd)
		return ;
	cmd = ctx->cmd;
	while (cmd)
	{
		close_cmd_file_descriptors(cmd);
		cmd = cmd->next;
	}
}
