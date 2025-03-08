/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_add.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 13:48:41 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/08 13:49:16 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Adds an argument to a command
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
	new_args = malloc(sizeof(char *) * (cmd->arg_count + 2));
	if (!new_args)
		return (-1);
	i = 0;
	while (i < cmd->arg_count)
	{
		new_args[i] = cmd->args[i];
		i++;
	}
	new_args[i] = ft_strdup(arg);
	new_args[i + 1] = NULL;
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
	redir->next = NULL;
	if (!cmd->redirections)
		cmd->redirections = redir;
	else
	{
		current = cmd->redirections;
		while (current->next)
			current = current->next;
		current->next = redir;
	}
	return (0);
}
