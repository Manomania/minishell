/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_command_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 11:26:48 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/24 11:27:13 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Adds an argument to a command
 *
 * This function adds a new argument to a command's argument list,
 * allocating memory as needed.
 *
 * @param cmd Command to add the argument to
 * @param value Value of the argument to add
 * @return 1 on success, 0 on failure
 */
int	add_argument(t_command *cmd, char *value)
{
	char	**new_args;
	int		i;

	if (!cmd || !value)
		return (0);
	new_args = malloc(sizeof(char *) * (size_t)(cmd->arg_count + 2));
	if (!new_args)
		return (0);
	i = -1;
	while (++i < cmd->arg_count)
		new_args[i] = cmd->args[i];
	new_args[i] = ft_strdup(value);
	if (!new_args[i])
	{
		free(new_args);
		return (0);
	}
	new_args[i + 1] = NULL;
	if (cmd->args)
		free(cmd->args);
	cmd->args = new_args;
	cmd->arg_count++;
	return (1);
}
