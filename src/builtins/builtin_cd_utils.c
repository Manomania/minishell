/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 16:47:07 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/28 10:05:03 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"

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
	(void)error(NULL, "cd", ERR_NO_OLDPWD);
	return (NULL);
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
	(void)error(NULL, "cd", ERR_NO_HOME);
	return (NULL);
}

/**
 * @brief Updates PWD environment variable
 *
 * @param env_node Current environment node
 * @param new_pwd New PWD value
 * @return t_env* Pointer to OLDPWD node if found, NULL otherwise
 */
t_env	*update_pwd_variable(t_env *env_node, char *new_pwd)
{
	t_env	*old_pwd_node;

	old_pwd_node = NULL;
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
	return (old_pwd_node);
}

/**
 * @brief Updates OLDPWD environment variable
 *
 * @param old_pwd_node OLDPWD environment node
 * @param old_pwd Old PWD value
 * @return void
 */
void	update_oldpwd_variable(t_env *old_pwd_node, char *old_pwd)
{
	if (old_pwd_node && old_pwd)
	{
		free(old_pwd_node->value);
		old_pwd_node->value = old_pwd;
	}
	else if (old_pwd)
		free(old_pwd);
}

/**
 * @brief Determines the target directory for cd command
 *
 * @param ctx Context for shell environment
 * @param cmd Command containing arguments
 * @return char* Target directory path (must be freed by caller)
 */
char	*get_target_directory(t_ctx *ctx, t_command *cmd)
{
	char	*target_dir;

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
	return (target_dir);
}
