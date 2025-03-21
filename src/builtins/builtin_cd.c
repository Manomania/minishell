/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 10:01:40 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/21 10:02:06 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "debug.h"
#include "error.h"
#include "minishell.h"

/**
 * @brief Updates PWD and OLDPWD environment variables
 *
 * @param ctx Context for shell environment
 * @param old_pwd Old PWD value
 * @return void
 */
static void	update_pwd_env(t_ctx *ctx, char *old_pwd)
{
	char	*new_pwd;
	t_env	*env_node;
	t_env	*old_pwd_node;

	new_pwd = getcwd(NULL, 0);
	if (!new_pwd)
	{
		error_print(ERROR, "cd", "Failed to get current directory");
		return ;
	}
	old_pwd_node = NULL;
	env_node = ctx->env_list;
	while (env_node)
	{
		if (ft_strncmp(env_node->key, "PWD", ft_strlen("PWD") + 1) == 0)
		{
			free(env_node->value);
			env_node->value = new_pwd;
		}
		else if (ft_strncmp(env_node->key, "OLDPWD", ft_strlen("OLDPWD")
				+ 1) == 0)
			old_pwd_node = env_node;
		env_node = env_node->next;
	}
	if (old_pwd_node && old_pwd)
	{
		free(old_pwd_node->value);
		old_pwd_node->value = old_pwd;
	}
	else if (old_pwd)
		free(old_pwd);
}

/**
 * @brief Gets the home directory from environment
 *
 * @param ctx Context for shell environment
 * @return char* Home directory or NULL if not found
 */
static char	*get_home_dir(t_ctx *ctx)
{
	t_env	*env_node;

	env_node = ctx->env_list;
	while (env_node)
	{
		if (ft_strncmp(env_node->key, "HOME", ft_strlen("HOME") + 1) == 0)
			return (ft_strdup(env_node->value));
		env_node = env_node->next;
	}
	error_print(ERROR, "cd", "HOME not set");
	return (NULL);
}

/**
 * @brief Gets the old PWD directory from environment
 *
 * @param ctx Context for shell environment
 * @return char* Old PWD or NULL if not found
 */
static char	*get_old_pwd(t_ctx *ctx)
{
	t_env	*env_node;

	env_node = ctx->env_list;
	while (env_node)
	{
		if (ft_strncmp(env_node->key, "OLDPWD", ft_strlen("OLDPWD") + 1) == 0)
			return (ft_strdup(env_node->value));
		env_node = env_node->next;
	}
	error_print(ERROR, "cd", "OLDPWD not set");
	return (NULL);
}

/**
 * @brief Gets the current PWD directory from environment
 *
 * @param ctx Context for shell environment
 * @return char* Current PWD or NULL if not found
 */
static char	*get_current_pwd(t_ctx *ctx)
{
	char	*pwd;

	pwd = getcwd(NULL, 0);
	if (pwd)
		return (pwd);
	error_print(WARNING, "cd", "Failed to get current directory");
	pwd = env_find(ctx, (char *)"PWD=");
	if (pwd)
		return (pwd);
	return (ft_strdup(""));
}

/**
 * @brief Executes the cd built-in command
 *
 * @param ctx Context for shell environment
 * @param cmd Command containing arguments
 * @return int Exit status (0 for success, non-zero for error)
 */
int	builtin_cd(t_ctx *ctx, t_command *cmd)
{
	char	*target_dir;
	char	*old_pwd;
	int		result;

	debug_log(DEBUG_INFO, "builtin", "Executing cd builtin");
	old_pwd = get_current_pwd(ctx);
	if (cmd->arg_count == 0 || ft_strncmp(cmd->args[1], "~", 2) == 0)
		target_dir = get_home_dir(ctx);
	else if (ft_strncmp(cmd->args[1], "-", 2) == 0)
	{
		target_dir = get_old_pwd(ctx);
		if (target_dir)
			ft_printf("%s\n", target_dir);
	}
	else
		target_dir = ft_strdup(cmd->args[1]);
	if (!target_dir)
		return (free(old_pwd), 1);
	result = chdir(target_dir);
	if (result != 0)
	{
		error_print(ERROR, "cd", strerror(errno));
		free(target_dir);
		free(old_pwd);
		return (1);
	}
	free(target_dir);
	update_pwd_env(ctx, old_pwd);
	return (0);
}
