/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_add.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 13:48:41 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/08 13:44:05 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Creates a new arguments array with space for one more argument
 *
 * @param cmd Command containing the current arguments
 * @return char** New array or NULL on failure
 */
static char	**create_new_args_array(t_command *cmd)
{
	char	**new_args;
	int		i;

	new_args = malloc(sizeof(char *) * (cmd->arg_count + 3));
	if (!new_args)
		return (NULL);
	i = 0;
	while (i <= cmd->arg_count)
	{
		new_args[i] = cmd->args[i];
		i++;
	}
	new_args[cmd->arg_count + 1] = NULL;
	new_args[cmd->arg_count + 2] = NULL;
	return (new_args);
}

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
	char	*arg_copy;

	if (!cmd || !arg)
		return (-1);
	new_args = create_new_args_array(cmd);
	if (!new_args)
		return (-1);
	arg_copy = ft_strdup(arg);
	if (!arg_copy)
	{
		free(new_args);
		return (-1);
	}
	new_args[cmd->arg_count + 1] = arg_copy;
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
 * @param filename Target filename
 * @return int 0 on success, -1 on failure
 */
int	command_add_redirection(t_command *cmd, t_token_type type, char *filename)
{
	t_redirection	*redir;
	t_redirection	*current;

	if (!cmd || !filename)
		return (-1);
	redir = malloc(sizeof(t_redirection));
	if (!redir)
		return (-1);
	redir->type = type;
	redir->fd = -1;
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
