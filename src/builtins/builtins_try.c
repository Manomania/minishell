/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_try.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 12:19:14 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/15 14:11:03 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "debug.h"
#include "error.h"
#include "minishell.h"

/**
 * Pre-creates any output files for redirections
 *
 * @param cmd Command with redirections
 * @return 0 on success, -1 on failure
 */
static int	prepare_builtin_files(t_command *cmd)
{
	t_redirection	*redir;
	int				fd;

	redir = cmd->redirection;
	while (redir)
	{
		if (redir->type == TOK_REDIR_TO || redir->type == TOK_HERE_DOC_TO)
		{
			fd = open_redirection_file(redir);
			if (fd != -1)
			{
				/* Truncate file if using > */
				if (redir->type == TOK_REDIR_TO)
					write(fd, "", 0);
				close(fd);
			}
			else
				return (-1);
		}
		redir = redir->next;
	}
	return (0);
}

/**
 * Executes a built-in command with proper redirection handling
 *
 * @param ctx Context for shell environment
 * @param cmd Command to execute
 * @param exit_status Pointer to store exit status
 * @return t_bool true if a built-in was executed, false otherwise
 */
static t_bool	execute_builtin(t_ctx *ctx, t_command *cmd, int *exit_status)
{
	int	saved_fds[2];
	int	redir_status;

	/* Create output files first to ensure they exist */
	if (prepare_builtin_files(cmd) == -1)
	{
		*exit_status = 1;
		return (1);
	}
	saved_fds[0] = dup(STDIN_FILENO);
	saved_fds[1] = dup(STDOUT_FILENO);
	if (saved_fds[0] == -1 || saved_fds[1] == -1)
	{
		if (saved_fds[0] != -1)
			close(saved_fds[0]);
		if (saved_fds[1] != -1)
			close(saved_fds[1]);
		(void)error(NULL, "execute_builtin", ERR_IO);
		return (0);
	}
	redir_status = setup_builtin_redirections(cmd, saved_fds);
	if (redir_status == -1)
	{
		*exit_status = 1;
		builtin_restore_redirections(saved_fds);
		return (1);
	}
	run_builtin_command(ctx, cmd, exit_status);
	builtin_restore_redirections(saved_fds);
	return (1);
}

/**
 * Checks if the command is a recognized builtin
 *
 * @param cmd_name Name of the command to check
 * @return t_bool true if command is a builtin, false otherwise
 */
static t_bool	is_recognized_builtin(char *cmd_name)
{
	if (is_builtin(cmd_name, (char *)"exit") == 0 || is_builtin(cmd_name,
			(char *)"echo") == 0 || is_builtin(cmd_name, (char *)"cd") == 0
		|| is_builtin(cmd_name, (char *)"pwd") == 0 || is_builtin(cmd_name,
			(char *)"export") == 0 || is_builtin(cmd_name, (char *)"unset") == 0
		|| is_builtin(cmd_name, (char *)"env") == 0)
		return (1);
	return (0);
}

/**
 * Validates the command arguments
 *
 * @param ctx Context for shell environment
 * @param cmd Command to validate
 * @return t_bool true if valid, false otherwise
 */
static t_bool	validate_command(t_ctx *ctx, t_command *cmd)
{
	if (!ctx || !cmd || !cmd->args || !cmd->args[0])
		return (0);
	return (1);
}

/**
 * Tries to execute a builtin command
 *
 * @param ctx Context for shell environment
 * @param cmd Command to check for builtin
 * @return t_bool true if builtin was executed, false otherwise
 */
t_bool	builtins_try(t_ctx *ctx, t_command *cmd)
{
	int	exit_status;

	if (!validate_command(ctx, cmd))
		return (0);
	if (is_recognized_builtin(cmd->args[0]))
	{
		if (execute_builtin(ctx, cmd, &exit_status))
		{
			ctx->exit_status = exit_status;
			return (1);
		}
	}
	return (0);
}
