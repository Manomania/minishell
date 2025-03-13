/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_parse.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 13:46:45 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/13 11:51:10 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Handles redirection tokens and adds them to the command
 *
 * @param cmd Command to add redirection to
 * @param token Current redirection token
 * @param next_token The next token (containing filename)
 * @return int 0 on success, -1 on failure
 */
static int	handle_redirection(t_command *cmd, t_token *token,
		t_token *next_token)
{
	int	fd;
	int	result;

	if (!cmd || !token || !next_token)
		return (-1);
	if (next_token->type != TOK_WORD)
		return (-1);
	fd = 1;
	if (token->type == TOK_REDIR_FROM || token->type == TOK_HERE_DOC_FROM)
		fd = 0;
	result = command_add_redirection(cmd, token->type, fd, next_token->value);
	return (result);
}

/**
 * @brief Initialize command with first arg as command name
 * Command name goes to args[0] as per standard convention
 *
 * @param cmd Command structure to initialize
 * @param cmd_name Name of the command to set
 * @return t_bool Whether the function succeeded or not
 */
static t_bool	init_command(t_command *cmd, char *cmd_name)
{
	if (!cmd || !cmd_name)
		return (false);
	cmd->args = malloc(sizeof(char *) * 2);
	if (!cmd->args)
		return (false);
	cmd->args[0] = ft_strdup(cmd_name);
	if (!cmd->args[0])
	{
		free(cmd->args);
		cmd->args = NULL;
		return (false);
	}
	cmd->args[1] = NULL;
	cmd->arg_count = 0;
	return (true);
}

/**
 * @brief Process a word token during command parsing
 *
 * @param cmd Command structure being built
 * @param token Current token being processed
 * @return t_bool true on success, false on failure
 */
static t_bool	process_word_token(t_command *cmd, t_token *token)
{
	if (!cmd->args)
	{
		if (!init_command(cmd, token->value))
			return (false);
	}
	else if (command_add_argument(cmd, token->value) != 0)
		return (false);
	return (true);
}

/**
 * @brief Parses tokens into a command structure
 *
 * @param tokens The tokens array to parse
 * @return t_command* Parsed command structure
 */
t_command	*command_parse(t_token *tokens)
{
	t_command *cmd;
	t_token *current;

	cmd = command_new();
	if (!cmd)
		return (NULL);
	current = tokens;
	while (current && current->type != TOK_PIPE)
	{
		if (current->type == TOK_WORD)
		{
			if (!process_word_token(cmd, current))
				return (command_free(cmd), NULL);
		}
		else if (token_is_redirection(current->type))
		{
			if (current->next && handle_redirection(cmd, current,
					current->next) == -1)
				return (command_free(cmd), NULL);
			if (current->next)
				current = current->next;
		}
		if (current)
			current = current->next;
	}
	return (cmd);
}