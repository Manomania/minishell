/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipeline_utils2.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 15:26:43 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/02 14:00:32 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "minishell.h"

/**
 * Handles here-doc redirection in a child process
 *
 * @param redir Redirection to handle
 * @return 0 on success, -1 on error
 */
static int	apply_heredoc_redirection(t_redirection *redir)
{
	int	result;

	if (redir->fd >= 0)
	{
		result = dup2(redir->fd, STDIN_FILENO);
		close(redir->fd);
		redir->fd = -1;
		if (result == -1)
			return (-1);
	}
	return (0);
}

/**
 * Applies redirections for a specific command in a child process
 *
 * @param cmd Command containing redirections
 * @param ctx Context for exit status
 * @return 0 on success, -1 on error
 */
int	apply_child_redirections(t_command *cmd, t_ctx *ctx)
{
	t_redirection	*redir;
	int				result;

	if (!cmd || !cmd->redirection)
		return (0);
	redir = cmd->redirection;
	while (redir)
	{
		if (redir->type == TOK_HERE_DOC_FROM)
			result = apply_heredoc_redirection(redir);
		else
			result = apply_redirection(redir, ctx);
		if (result == -1)
			return (-1);
		redir = redir->next;
	}
	return (0);
}
