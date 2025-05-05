/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_pwd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 09:54:32 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/05 16:44:27 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
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

	(void)ctx;
	(void)cmd;
	pwd = getcwd(NULL, 0);
	if (!pwd)
		return (error(NULL, "pwd", ERR_NO_PWD));
	ft_printf("%s\n", pwd);
	free(pwd);
	return (0);
}
