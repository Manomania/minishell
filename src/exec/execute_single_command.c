/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_single_command.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 12:15:06 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/29 12:50:27 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "error.h"
#include "minishell.h"

/**
 * @brief Executes a builtin command with proper redirection
 *
 * @param ctx Context containing environment and command info
 * @param saved_stdin File descriptor for saved stdin
 * @param saved_stdout File descriptor for saved stdout
 * @return Exit status of the builtin command
 */
int	execute_builtin_command(t_ctx *ctx, int saved_stdin, int saved_stdout)
{
	int	exit_code;
	int	result;

	exit_code = 0;
	result = setup_command_redirections(ctx, ctx->cmd);
	if (result != 0)
		exit_code = 1;
	else
	{
		builtins_try(ctx, ctx->cmd);
		exit_code = ctx->exit_status;
	}
	if (saved_stdin != -1)
	{
		dup2(saved_stdin, STDIN_FILENO);
		close(saved_stdin);
	}
	if (saved_stdout != -1)
	{
		dup2(saved_stdout, STDOUT_FILENO);
		close(saved_stdout);
	}
	return (exit_code);
}

/**
 * @brief Creates environment array from environment list
 *
 * @param env_list Environment variable list
 * @return char** New environment array or NULL on error
 */
char	**create_env_array(t_env *env_list)
{
	t_env	*current;
	char	**env_array;
	int		count;
	int		i;

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
	i = 0;
	current = env_list;
	while (current)
	{
		if (current->value)
			env_array[i] = ft_strjoin(ft_strjoin(current->key, "="),
					current->value);
		else
			env_array[i] = ft_strdup(current->key);
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
 * @brief Function executed by child process
 *
 * @param ctx Context containing environment and command info
 * @param saved_stdin Saved standard input descriptor
 * @param saved_stdout Saved standard output descriptor
 */
static void	execute_child_process(t_ctx *ctx, int saved_stdin, int saved_stdout)
{
	char	**env_array;
	int		result;

	if (saved_stdin != -1)
		close(saved_stdin);
	if (saved_stdout != -1)
		close(saved_stdout);
	reset_signals();
	result = setup_command_redirections(ctx, ctx->cmd);
	if (result != 0)
	{
		cleanup_child_process(ctx);
		exit(1);
	}
	if (!command_bin(ctx))
	{
		if (ctx->cmd->args && ctx->cmd->args[0])
		{
			if (ft_strchr(ctx->cmd->args[0], '/'))
				exit(126);
			else
				exit(127);
		}
		cleanup_child_process(ctx);
		exit(127);
	}
	env_array = create_env_array(ctx->env_list);
	if (!env_array)
	{
		cleanup_child_process(ctx);
		exit(1);
	}
	execve(ctx->cmd->args[0], ctx->cmd->args, env_array);
	perror("execve");
	free_2d_array((void **)env_array);
	cleanup_child_process(ctx);
	exit(126);
}

/**
 * @brief Handles parent process waiting and signal processing
 *
 * @param pid Child process ID to wait for
 * @param saved_stdin Saved standard input descriptor
 * @param saved_stdout Saved standard output descriptor
 * @return Exit status of the command
 */
static int	handle_parent_process(pid_t pid, int saved_stdin, int saved_stdout)
{
	int	status;
	int	exit_code;

	if (saved_stdin != -1)
		close(saved_stdin);
	if (saved_stdout != -1)
		close(saved_stdout);
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		exit_code = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
	{
		exit_code = 128 + WTERMSIG(status);
		if (WTERMSIG(status) == SIGQUIT)
			ft_printf_fd(STDERR_FILENO, "Quit (core dumped)\n");
		else if (WTERMSIG(status) == SIGINT && isatty(STDOUT_FILENO))
			write(STDOUT_FILENO, "\n", 1);
	}
	else
		exit_code = 1;
	setup_signals();
	return (exit_code);
}

/**
 * @brief Executes an external command with fork
 *
 * @param ctx Context containing environment and command info
 * @param saved_stdin Saved standard input descriptor
 * @param saved_stdout Saved standard output descriptor
 * @return Exit status of the external command
 */
static int	execute_external_command(t_ctx *ctx, int saved_stdin,
		int saved_stdout)
{
	pid_t	pid;
	int		exit_code;

	setup_parent_signals();
	pid = fork();
	if (pid == -1)
	{
		if (saved_stdin != -1)
			close(saved_stdin);
		if (saved_stdout != -1)
			close(saved_stdout);
		setup_signals();
		return (error(NULL, "fork", ERR_CHILD));
	}
	if (pid == 0)
		execute_child_process(ctx, saved_stdin, saved_stdout);
	exit_code = handle_parent_process(pid, saved_stdin, saved_stdout);
	return (exit_code);
}

/**
 * @brief Executes a single command without a pipeline
 *
 * @param ctx Context containing environment and command info
 * @return Exit status of the command
 */
int	execute_single_command(t_ctx *ctx)
{
	int		saved_stdin;
	int		saved_stdout;
	char	*arg;

	saved_stdin = dup(STDIN_FILENO);
	saved_stdout = dup(STDOUT_FILENO);
	if (saved_stdin == -1 || saved_stdout == -1)
	{
		if (saved_stdin != -1)
			close(saved_stdin);
		if (saved_stdout != -1)
			close(saved_stdout);
		return (error(NULL, "dup", ERR_FD));
	}
	arg = NULL;
	if (ctx->cmd->args)
		arg = ctx->cmd->args[0];
	if (is_builtin_command(arg))
		return (execute_builtin_command(ctx, saved_stdin, saved_stdout));
	else
		return (execute_external_command(ctx, saved_stdin, saved_stdout));
}
