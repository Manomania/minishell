/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_execute_utils.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 17:54:30 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/10 15:33:41 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "debug.h"
#include "error.h"
#include "minishell.h"

/**
 * @brief Sets up child process redirections
 *
 * This function handles heredocs and redirections in the child process.
 *
 * @param ctx Context with environment information
 * @param cmd Command containing redirections
 * @return 0 on success, non-zero on error
 */
static int	setup_child_redirections(t_ctx *ctx, t_command *cmd)
{
	int	result;

	result = setup_heredocs(ctx, cmd);
	if (result != 0)
		return (result);
	result = setup_redirections(cmd->redirection);
	return (result);
}

/**
 * @brief Clean up all resources before child process exit
 *
 * @param ctx Context to clean up
 */
static void	child_process_cleanup(t_ctx *ctx)
{
	cleanup_heredoc_resources(ctx);
	if (ctx->tokens)
	{
		free_all_token(ctx->tokens);
		ctx->tokens = NULL;
	}
	ctx_clear(ctx);
}

t_bool	has_only_redirections(t_command *cmd)
{
	return (cmd && cmd->redirection && (!cmd->args || !cmd->args[0]));
}

/**
 * @brief Creates environment array from environment list
 *
 * @param env_list Environment variable list
 * @return char** New environment array or NULL on error
 */
static char	**create_env_array(t_env *env_list)
{
	t_env	*current;
	char	**env_array;
	int		count;
	int		i;
	char	*tmp;

	count = 0;
	current = env_list;
	while (current)
	{
		count++;
		current = current->next;
	}
	env_array = malloc(sizeof(char *) * (count + 1));
	if (!env_array)
		return (NULL);
	current = env_list;
	i = 0;
	while (current)
	{
		tmp = ft_strjoin(current->key, "=");
		if (!tmp)
		{
			free_2d_array((void **)env_array);
			return (NULL);
		}
		env_array[i] = ft_strjoin(tmp, current->value ? current->value : "");
		free(tmp);
		if (!env_array[i])
		{
			free_2d_array((void **)env_array);
			return (NULL);
		}
		i++;
		current = current->next;
	}
	env_array[i] = NULL;
	return (env_array);
}

/**
 * Sets up and manages child process redirection
 *
 * @param ctx Context containing shell state
 * @return int Status code, 0 for success or error code
 */
static int	handle_child_redirections(t_ctx *ctx)
{
	int	redirect_result;

	redirect_result = setup_child_redirections(ctx, ctx->cmd);
	if (redirect_result != 0)
		return (EXIT_FAILURE);
	if (!ctx->cmd->args || !ctx->cmd->args[0])
		return (EXIT_SUCCESS);
	return (0);
}

/**
 * Validates command binary and handles error reporting
 *
 * @param ctx Context containing shell state
 * @return int 0 if valid, error code otherwise
 */
static int	validate_child_command(t_ctx *ctx)
{
	if (!command_bin(ctx))
	{
		if (ctx->cmd->args && ctx->cmd->args[0])
		{
			if (ctx->cmd->args[0][0] == '.' && !ft_strchr(ctx->cmd->args[0],
					'/'))
				return (error_code(ERR_NO_FILE));
			if (ft_strchr(ctx->cmd->args[0], '/'))
				return (error_code(ERR_IS_DIR));
			return (error(ctx->cmd->args[0], "minishell", ERR_CMD_NOT_FOUND));
		}
		return (error_code(ERR_CMD_NOT_FOUND));
	}
	return (0);
}

/**
 * Main child process execution function
 *
 * @param ctx Context containing shell state
 */
void	execute_child(t_ctx *ctx)
{
	int		status;
	char	**env_array;

	reset_signals();
	status = handle_child_redirections(ctx);
	if (status != 0)
	{
		child_process_cleanup(ctx);
		exit(status);
	}
	status = validate_child_command(ctx);
	if (status != 0)
	{
		child_process_cleanup(ctx);
		exit(status);
	}
	env_array = create_env_array(ctx->env_list);
	if (!env_array)
	{
		child_process_cleanup(ctx);
		exit(EXIT_FAILURE);
	}
	execve(ctx->cmd->args[0], ctx->cmd->args, env_array);
	free_2d_array((void **)env_array);
	perror("execve");
	child_process_cleanup(ctx);
	exit(EXIT_FAILURE);
}

/**
 * @brief Gets the exit status from process termination information
 *
 * This function extracts the exit status from the process status.
 *
 * @param status Process status from waitpid
 * @return Exit status of the process
 */
int	get_exit_status(int status)
{
	int	exit_code;

	if (WIFEXITED(status))
	{
		exit_code = WEXITSTATUS(status);
		return (exit_code);
	}
	if (WIFSIGNALED(status))
	{
		exit_code = 128 + WTERMSIG(status);
		return (exit_code);
	}
	return (1);
}
