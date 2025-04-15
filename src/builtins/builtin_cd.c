/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 10:01:40 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/15 11:03:48 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "debug.h"
#include "error.h"
#include "minishell.h"

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
	else if (ft_strncmp(cmd->args[1], ".", 2) == 0)
		target_dir = handle_dot_directory(ctx);
	else
		target_dir = ft_strdup(cmd->args[1]);
	return (target_dir);
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
	char	*env_pwd;

	pwd = getcwd(NULL, 0);
	if (pwd)
		return (pwd);
	env_pwd = env_find(ctx, (char *)"PWD=");
	if (env_pwd)
		return (env_pwd);
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
	t_env	*old_pwd_node;

	if (cmd->arg_count > 1)
	{
		error(NULL, "cd", ERR_TOO_MANY_ARGS);
		return (1);
	}
	old_pwd = get_current_pwd(ctx);
	target_dir = get_target_directory(ctx, cmd);
	if (!target_dir)
		return (free(old_pwd), 1);
	result = chdir(target_dir);
	if (result != 0)
	{
		error(target_dir, "cd", ERR_NO_FILE);
		free(target_dir);
		free(old_pwd);
		return (1);
	}
	old_pwd_node = update_pwd_variable(ctx->env_list, getcwd(NULL, 0));
	update_oldpwd_variable(old_pwd_node, old_pwd);
	free(target_dir);
	return (0);
}
