/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_parse.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 13:46:45 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/17 13:52:41 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Processes a word token during command parsing
 *
 * @param cmd Command structure being built
 * @param token Current token being processed
 * @return t_bool true on success, false on failure
 */
static t_bool	process_word_token(t_command *cmd, t_token *token)
{
	if (!cmd->args)
	{
		cmd->args = malloc(sizeof(char *) * 2);
		if (!cmd->args)
			return (false);
		cmd->args[0] = ft_strdup(token->value);
		if (!cmd->args[0])
		{
			free(cmd->args);
			cmd->args = NULL;
			return (false);
		}
		cmd->args[1] = NULL;
		cmd->arg_count = 0;
	}
	else if (command_add_argument(cmd, token->value) != 0)
		return (false);
	return (true);
}

/**
 * @brief Processes a redirection token during command parsing
 *
 * @param cmd Command to add redirection to
 * @param token Current redirection token
 * @param next_token The next token (containing filename)
 * @return int 0 on success, -1 on failure
 */
static int	handle_redirection_token(t_command *cmd, t_token *token,
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
 * @brief Processes tokens until end of command or pipeline marker
 *
 * @param current Current token pointer reference
 * @param cmd Current command being built
 * @return t_bool true on success, false on failure
 */
static t_bool	process_command_tokens(t_token **current, t_command *cmd)
{
	while (*current && (*current)->type != TOK_PIPE)
	{
		if ((*current)->type == TOK_WORD)
		{
			if (!process_word_token(cmd, *current))
				return (false);
		}
		else if (token_is_redirection((*current)->type))
		{
			if ((*current)->next && handle_redirection_token(cmd, *current,
					(*current)->next) == -1)
				return (false);
			if ((*current)->next)
				*current = (*current)->next;
		}
		if (*current)
			*current = (*current)->next;
	}
	return (true);
}

/**
 * @brief Creates commands for a pipeline of commands
 *
 * @param cmd Current command pointer reference
 * @param current Current token pointer reference
 * @return t_bool true on success, false on failure
 */
static t_bool	create_pipeline(t_command **cmd, t_token **current)
{
	t_command	*new_cmd;

	if (*current && (*current)->type == TOK_PIPE)
	{
		new_cmd = command_new();
		if (!new_cmd)
			return (false);
		(*cmd)->next = new_cmd;
		*cmd = new_cmd;
		*current = (*current)->next;
		if (!process_command_tokens(current, *cmd))
			return (false);
		if (*current && (*current)->type == TOK_PIPE)
			return (create_pipeline(cmd, current));
	}
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
	t_command	*cmd;
	t_command	*first_cmd;
	t_token		*current;

	cmd = command_new();
	if (!cmd)
		return (NULL);
	first_cmd = cmd;
	current = tokens;
	if (!process_command_tokens(&current, cmd))
	{
		free_all_commands(first_cmd);
		return (NULL);
	}
	if (!create_pipeline(&cmd, &current))
	{
		free_all_commands(first_cmd);
		return (NULL);
	}
	return (first_cmd);
}
