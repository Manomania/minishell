/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_execute_utils.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 17:54:30 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/27 18:09:45 by elagouch         ###   ########.fr       */
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
 * @brief Checks if command exists and is executable
 *
 * This function verifies the command path and permissions.
 *
 * @param cmd Command to check
 * @return 0 on success, non-zero on error
 */
static int	check_command_executable(t_command *cmd)
{
	if (!cmd->args[0] || access(cmd->args[0], X_OK) != 0)
	{
		ft_printf("command not found or not executable: %s\n", cmd->args[0]);
		return (1);
	}
	return (0);
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
 * @brief Executes the child process portion of a command
 *
 * @param ctx Context containing environment and command info
 */
void	execute_child(t_ctx *ctx)
{
	int		redirect_result;
	int		cmd_check;
	char	**env_array;

	reset_signals();
	redirect_result = setup_child_redirections(ctx, ctx->cmd);
	if (redirect_result != 0)
		exit(EXIT_FAILURE);
	cmd_check = check_command_executable(ctx->cmd);
	if (cmd_check != 0)
		exit(EXIT_FAILURE);
	env_array = create_env_array(ctx->env_list);
	if (!env_array)
		exit(EXIT_FAILURE);
	execve(ctx->cmd->args[0], ctx->cmd->args, env_array);
	free_2d_array((void **)env_array);
	perror("execve");
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
