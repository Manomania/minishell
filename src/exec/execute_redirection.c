/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_redirection.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 12:18:29 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/02 14:05:49 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"

/**
 * @brief Opens a file for redirection
 *
 * @param redir Redirection information
 * @return int File descriptor or -1 on error
 */
int	open_redirection_file(t_redirection *redir)
{
	int	fd;
	int	flags;

	if (redir->type == TOK_HERE_DOC_FROM)
	{
		if (redir->fd >= 0)
			return (redir->fd);
		return (-1);
	}
	if (redir->type == TOK_REDIR_FROM)
		flags = O_RDONLY;
	else if (redir->type == TOK_REDIR_TO)
		flags = O_WRONLY | O_CREAT | O_TRUNC;
	else
		flags = O_WRONLY | O_CREAT | O_APPEND;
	fd = open(redir->filename, flags, 0644);
	return (fd);
}

/**
 * @brief Applies a specific redirection
 *
 * @param redir Redirection to apply
 * @param ctx Context for exit status
 * @return int 0 on success, -1 on error
 */
int	apply_redirection(t_redirection *redir, t_ctx *ctx)
{
	int	fd;
	int	result;
	int	error_val;

	fd = open_redirection_file(redir);
	if (fd == -1)
	{
		error_val = errno;
		if (error_val == EISDIR)
			ctx->exit_status = error(redir->filename, NULL, ERR_IS_DIR);
		else if (error_val == EACCES)
			ctx->exit_status = error(redir->filename, NULL, ERR_NO_PERMS);
		else
			ctx->exit_status = error(redir->filename, NULL, ERR_NO_FILE);
		return (-1);
	}
	if (redir->type == TOK_REDIR_FROM || redir->type == TOK_HERE_DOC_FROM)
		result = dup2(fd, STDIN_FILENO);
	else
		result = dup2(fd, STDOUT_FILENO);
	close(fd);
	if (result == -1)
	{
		ctx->exit_status = error(redir->filename, NULL, ERR_FD);
		return (-1);
	}
	return (0);
}

/**
 * @brief Sets up redirections for a command
 *
 * @param redirections List of redirections to set up
 * @param ctx Context for exit status
 * @return int 0 on success, -1 on error
 */
int	setup_redirections(t_redirection *redirections, t_ctx *ctx)
{
	t_redirection	*redir;

	if (!redirections)
		return (0);
	redir = redirections;
	while (redir)
	{
		if (apply_redirection(redir, ctx) == -1)
			return (-1);
		redir = redir->next;
	}
	return (0);
}

/**
 * @brief Sets up all redirections for a command
 *
 * @param ctx Context containing environment info
 * @param cmd Command containing redirections
 * @return int 0 on success, non-zero on error
 */
int	setup_command_redirections(t_ctx *ctx, t_command *cmd)
{
	if (setup_heredocs(ctx, cmd) != 0)
		return (1);
	if (setup_redirections(cmd->redirection, ctx) != 0)
		return (1);
	return (0);
}

/**
 * @brief Reads heredoc content for all commands in a pipeline
 *
 * @param ctx Context containing environment information
 * @return int 0 on success, -1 on error
 */
int	read_all_heredocs(t_ctx *ctx)
{
	t_command		*current;
	t_redirection	*redir;
	int				heredoc_fd;

	current = ctx->cmd;
	while (current)
	{
		redir = current->redirection;
		while (redir)
		{
			if (redir->type == TOK_HERE_DOC_FROM)
			{
				heredoc_fd = create_heredoc(ctx, redir->filename);
				if (heredoc_fd == -1)
					return (-1);
				redir->fd = heredoc_fd;
			}
			redir = redir->next;
		}
		current = current->next;
	}
	return (0);
}
