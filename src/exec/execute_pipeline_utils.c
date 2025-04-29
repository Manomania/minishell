/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipeline_utils.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 15:52:23 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/29 17:36:02 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "error.h"
#include "minishell.h"

/**
 * @brief Counts the number of commands in a pipeline
 *
 * @param cmd The first command in the pipeline
 * @return int The number of commands
 */
int	count_commands(t_command *cmd)
{
	int			count;
	t_command	*current;

	count = 0;
	current = cmd;
	while (current)
	{
		count++;
		current = current->next;
	}
	return (count);
}

/**
 * @brief Handles command execution after redirection setup
 *
 * @param ctx Context with environment
 * @param cmd Command to execute
 */
void	execute_command(t_ctx *ctx, t_command *cmd)
{
	char	*bin_path;

	if (builtins_try(ctx, cmd))
	{
		cleanup_child_process(ctx);
		exit(EXIT_SUCCESS);
	}
	bin_path = validate_and_resolve_command(ctx, cmd);
	if (!bin_path && (!cmd->args || !cmd->args[0]))
	{
		cleanup_child_process(ctx);
		exit(EXIT_SUCCESS);
	}
	else if (!bin_path)
	{
		cleanup_child_process(ctx);
		exit(127);
	}
	free(cmd->args[0]);
	cmd->args[0] = bin_path;
	if (execve(cmd->args[0], cmd->args, ctx->envp) == -1)
		ctx_error_exit(ctx, cmd->args[0], "exec", ERR_CHILD);
}

/**
 * Handles redirections and pipe setup for pipeline commands
 *
 * @param cmd Command to process
 * @param input_fd Input file descriptor
 * @param output_fd Output file descriptor
 * @return 0 on success, -1 on error
 */
int	setup_child_pipeline_redirections(t_command *cmd, int input_fd,
		int output_fd)
{
	if (input_fd != STDIN_FILENO)
	{
		if (dup2(input_fd, STDIN_FILENO) == -1)
		{
			close(input_fd);
			return (-1);
		}
		close(input_fd);
	}
	if (output_fd != STDOUT_FILENO)
	{
		if (dup2(output_fd, STDOUT_FILENO) == -1)
		{
			close(output_fd);
			return (-1);
		}
		close(output_fd);
	}
	return (apply_child_redirections(cmd));
}

/**
 * @brief Clean up resources before child process exit
 *
 * @param ctx Context containing all resources
 */
void	cleanup_child_process(t_ctx *ctx)
{
	cleanup_heredoc_resources(ctx);
	if (ctx->tokens)
	{
		free_all_token(ctx->tokens);
		ctx->tokens = NULL;
	}
	ctx_clear(ctx);
}

/**
 * @brief Resolves command binary path
 *
 * Determines if command is a builtin or external command and resolves its path.
 *
 * @param ctx Shell context
 * @param cmd Command to validate
 * @return Command path or NULL on error (with error displayed)
 */
char	*validate_and_resolve_command(t_ctx *ctx, t_command *cmd)
{
	char	*bin_path;

	if (!cmd->args || !cmd->args[0])
		return (NULL);
	if (is_builtin_command(cmd->args[0]))
		return (ft_strdup(cmd->args[0]));
	bin_path = bin_find(ctx, cmd->args[0]);
	if (!bin_path && is_path(cmd->args[0]))
		return (NULL);
	else if (!bin_path)
	{
		ft_printf_fd(STDERR_FILENO, "minishell: %s: command not found\n",
			cmd->args[0]);
	}
	return (bin_path);
}
