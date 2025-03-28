/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 10:01:40 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/28 11:24:24 by elagouch         ###   ########.fr       */
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
		(void)error(NULL, "cd", ERR_NO_PWD);
		return ;
	}
	env_node = ctx->env_list;
	old_pwd_node = update_pwd_variable(env_node, new_pwd);
	update_oldpwd_variable(old_pwd_node, old_pwd);
}

/**
 * @brief Changes directory to the target path
 *
 * @param target_dir Target directory path
 * @param old_pwd Old PWD value (will be freed on error)
 * @return int Exit status (0 for success, 1 for error)
 */
int	change_directory(char *target_dir, char *old_pwd)
{
	int	result;

	if (!target_dir)
		return (free(old_pwd), 1);
	result = chdir(target_dir);
	if (result != 0)
	{
		(void)error(NULL, "cd", ERR_NO_FILE);
		free(target_dir);
		free(old_pwd);
		return (1);
	}
	free(target_dir);
	return (0);
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
	ft_printf_fd(STDERR_FILENO,
		YELLOW "minishell: warning: cd: failed to get current directory" RESET);
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

	old_pwd = get_current_pwd(ctx);
	target_dir = get_target_directory(ctx, cmd);
	result = change_directory(target_dir, old_pwd);
	if (result != 0)
		return (1);
	update_pwd_env(ctx, old_pwd);
	return (0);
}
