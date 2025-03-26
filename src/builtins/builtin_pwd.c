/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_pwd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 09:54:32 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/21 10:02:23 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "debug.h"
#include "error.h"
#include "minishell.h"

/**
 * @brief Executes the pwd built-in command
 *
 * @param ctx Context for shell environment
 * @param cmd Command containing arguments
 * @return int Exit status (0 for success, non-zero for error)
 */
int	builtin_pwd(t_ctx *ctx, t_command *cmd)
{
	char	*pwd;

	(void)cmd;
	debug_log(DEBUG_INFO, "builtin", "Executing pwd builtin");
	pwd = getcwd(NULL, 0);
	if (!pwd)
	{
		pwd = env_find(ctx, (char *)"PWD=");
		if (!pwd)
		{
			error_print(ERROR, "pwd", "Failed to get current directory");
			return (1);
		}
	}
	ft_printf("%s\n", pwd);
	free(pwd);
	return (0);
}
