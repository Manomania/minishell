/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_cleanup.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 19:26:26 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/07 19:26:43 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Ensures all heredoc resources are properly freed
 *
 * @param ctx Context containing tokens and commands
 * @return void
 */
void	cleanup_heredoc_resources(t_ctx *ctx)
{
	t_command		*cmd;
	t_redirection	*redir;
	t_redirection	*next_redir;

	if (!ctx || !ctx->cmd)
		return ;
	cmd = ctx->cmd;
	while (cmd)
	{
		redir = cmd->redirection;
		while (redir)
		{
			next_redir = redir->next;
			if (redir->type == TOK_HERE_DOC_FROM)
			{
				free(redir->filename);
				free(redir);
			}
			redir = next_redir;
		}
		cmd = cmd->next;
	}
}
