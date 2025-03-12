/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_add.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 13:48:41 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/12 17:15:14 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Adds an argument to a command
 * Arguments are stored in args array starting at index 1
 * args[0] is always the command name
 *
 * @param cmd Command to add argument to
 * @param arg Argument string to add
 * @return int 0 on success, -1 on failure
 */
int	command_add_argument(t_command *cmd, char *arg)
{
	char	**new_args;
	int		i;

	if (!cmd || !arg)
		return (-1);
	new_args = malloc(sizeof(char *) * (cmd->arg_count + 2 + 1));
	if (!new_args)
		return (-1);
	new_args[0] = cmd->args[0];
	i = 0;
	while (i < cmd->arg_count)
	{
		new_args[i + 1] = cmd->args[i + 1];
		i++;
	}
	new_args[i + 1] = ft_strdup(arg);
	if (!new_args[i + 1])
	{
		free(new_args);
		return (-1);
	}
	new_args[i + 2] = NULL;
	if (cmd->args)
		free(cmd->args);
	cmd->args = new_args;
	cmd->arg_count++;
	return (0);
}

/**
 * @brief Adds a redirection to a command
 *
 * @param cmd Command to add redirection to
 * @param type Redirection type (< > << >>)
 * @param fd File descriptor (0 for input, 1 for output)
 * @param filename Target filename
 * @return int 0 on success, -1 on failure
 */
int	command_add_redirection(t_command *cmd, t_token_type type, int fd,
		char *filename)
{
	t_redirection	*redir;
	t_redirection	*current;

	if (!cmd || !filename)
		return (-1);
	redir = malloc(sizeof(t_redirection));
	if (!redir)
		return (-1);
	redir->type = type;
	redir->fd = fd;
	redir->filename = ft_strdup(filename);
	if (!redir->filename)
		return (free(redir), -1);
	redir->next = NULL;
	if (!cmd->redirection)
		cmd->redirection = redir;
	else
	{
		current = cmd->redirection;
		while (current->next)
			current = current->next;
		current->next = redir;
	}
	return (0);
}
