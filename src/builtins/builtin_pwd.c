/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_pwd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 09:54:32 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/05 18:57:10 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "error.h"
#include "minishell.h"

/**
 * @brief Executes the pwd built-in command
 *
 * @return int Exit status (0 for success, non-zero for error)
 */
int	builtin_pwd(void)
{
	char	*pwd;

	pwd = getcwd(NULL, 0);
	if (!pwd)
		return (error(NULL, "pwd", ERR_NO_PWD));
	ft_printf("%s\n", pwd);
	free(pwd);
	return (0);
}
