/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmdas_utils4.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 11:30:00 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/15 11:40:55 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "error.h"
#include "minishell.h"

/**
 * @brief Opens and prepares all files for redirection before forking
 *
 * @param cmd Command containing redirections
 * @return int 0 on success, -1 on error
 */
int	prepare_redirections_files(t_command *cmd)
{
	t_redirection	*redir;
	int				fd;

	if (!cmd || !cmd->redirection)
		return (0);
	redir = cmd->redirection;
	while (redir)
	{
		if (redir->type != TOK_HERE_DOC_FROM)
		{
			fd = open_redirection_file(redir);
			if (fd == -1)
				return (-1);
			close(fd);
		}
		redir = redir->next;
	}
	return (0);
}

/**
 * @brief Opens all output files in pipeline to ensure they're created
 *
 * @param cmd First command in the pipeline
 * @return int 0 on success, -1 on error
 */
int	prepare_all_pipeline_files(t_command *cmd)
{
	t_command	*current;
	int			result;

	current = cmd;
	while (current)
	{
		result = prepare_redirections_files(current);
		if (result != 0)
			return (result);
		current = current->next;
	}
	return (0);
}

/**
 * @brief Handles here-doc redirection in a child process
 *
 * @param redir Redirection to handle
 * @return int 0 on success, -1 on error
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
 * @brief Applies redirections for a specific command in a child process
 *
 * @param cmd Command containing redirections
 * @return int 0 on success, -1 on error
 */
int	apply_child_redirections(t_command *cmd)
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
			result = apply_redirection(redir);
		if (result == -1)
			return (-1);
		redir = redir->next;
	}
	return (0);
}

/**
 * @brief Handles redirections and pipe setup for pipeline commands
 *
 * @param cmd Command to process
 * @param input_fd Input file descriptor
 * @param output_fd Output file descriptor
 * @return int 0 on success, -1 on error
 */
int	setup_child_pipeline_redirections(t_command *cmd, int input_fd,
		int output_fd)
{
	if (input_fd != STDIN_FILENO)
	{
		if (dup2(input_fd, STDIN_FILENO) == -1)
			return (-1);
		close(input_fd);
	}
	if (output_fd != STDOUT_FILENO)
	{
		if (dup2(output_fd, STDOUT_FILENO) == -1)
			return (-1);
		close(output_fd);
	}
	return (apply_child_redirections(cmd));
}
