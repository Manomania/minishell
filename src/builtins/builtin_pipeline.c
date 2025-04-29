/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_pipeline.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 10:20:14 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/29 12:45:55 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "error.h"
#include "minishell.h"

/**
 * @brief Checks if a command is a built-in
 *
 * @param cmd_name Command name to check
 * @return t_bool true if built-in, false otherwise
 */
t_bool	is_builtin_command(char *cmd_name)
{
	if (!cmd_name)
		return (false);
	if (ft_strncmp(cmd_name, "exit", __INT_MAX__) == 0)
		return (true);
	if (ft_strncmp(cmd_name, "echo", __INT_MAX__) == 0)
		return (true);
	if (ft_strncmp(cmd_name, "cd", __INT_MAX__) == 0)
		return (true);
	if (ft_strncmp(cmd_name, "pwd", __INT_MAX__) == 0)
		return (true);
	if (ft_strncmp(cmd_name, "export", __INT_MAX__) == 0)
		return (true);
	if (ft_strncmp(cmd_name, "unset", __INT_MAX__) == 0)
		return (true);
	if (ft_strncmp(cmd_name, "env", __INT_MAX__) == 0)
		return (true);
	return (false);
}
