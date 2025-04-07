/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_parse.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 13:46:45 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/07 19:14:42 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Processes a redirection token during command parsing
 *
 * @param cmd Command to add redirection to
 * @param token Current redirection token
 * @param next_token The next token (containing filename)
 * @param ctx Context containing environment information
 * @return int 0 on success, -1 on failure
 */
static int	handle_redirection_token(t_command *cmd, t_token *token,
		t_token *next_token, t_ctx *ctx)
{
	int		fd;
	int		result;
	char	*expanded_filename;

	if (!cmd || !token || !next_token)
		return (-1);
	if (next_token->type != TOK_WORD)
		return (-1);
	fd = 1;
	if (token->type == TOK_REDIR_FROM || token->type == TOK_HERE_DOC_FROM)
		fd = 0;
	expanded_filename = handle_quotes_and_vars(ctx, next_token->value);
	if (!expanded_filename)
		return (-1);
	result = command_add_redirection(cmd, token->type, fd, expanded_filename);
	free(expanded_filename);
	return (result);
}

/**
 * @brief Manages empty command when first arg is empty string
 *
 * @param cmd Command being processed
 * @param current Token list being processed
 * @param ctx Context containing environment information
 * @return t_bool true if successfully updated command
 */
static t_bool	handle_empty_first_arg(t_command *cmd, t_token **current,
		t_ctx *ctx)
{
	if (*current && (*current)->type == TOK_WORD)
	{
		if (!process_word_token(cmd, *current, ctx))
			return (false);
		*current = (*current)->next;
	}
	return (true);
}

/**
 * @brief Processes tokens until end of command or pipeline marker
 *
 * @param current Current token pointer reference
 * @param cmd Current command being built
 * @param ctx Context containing environment information
 * @return t_bool true on success, false on failure
 */
static t_bool	process_command_tokens(t_token **current, t_command *cmd,
		t_ctx *ctx)
{
	char	*expanded_value;
	t_bool	first_arg_processed;
	t_bool	has_redirections;

	first_arg_processed = false;
	has_redirections = false;
	while (*current && (*current)->type != TOK_PIPE)
	{
		if ((*current)->type == TOK_WORD)
		{
			expanded_value = handle_quotes_and_vars(ctx, (*current)->value);
			if (!expanded_value)
				return (false);
			if (!first_arg_processed && expanded_value[0] == '\0')
			{
				free(expanded_value);
				*current = (*current)->next;
				first_arg_processed = true;
				if (!handle_empty_first_arg(cmd, current, ctx))
					return (false);
				continue ;
			}
			free(expanded_value);
			if (!process_word_token(cmd, *current, ctx))
				return (false);
			first_arg_processed = true;
		}
		else if (token_is_redirection((*current)->type))
		{
			if ((*current)->next && handle_redirection_token(cmd, *current,
					(*current)->next, ctx) == -1)
				return (false);
			has_redirections = true;
			if ((*current)->next)
				*current = (*current)->next;
		}
		if (*current)
			*current = (*current)->next;
	}
	return (first_arg_processed || has_redirections);
}

/**
 * @brief Creates commands for a pipeline of commands
 *
 * @param cmd Current command pointer reference
 * @param current Current token pointer reference
 * @param ctx Context containing environment information
 * @return t_bool true on success, false on failure
 */
static t_bool	create_pipeline(t_command **cmd, t_token **current, t_ctx *ctx)
{
	t_command	*new_cmd;
	t_command	*prev_cmd;

	prev_cmd = *cmd;
	new_cmd = command_new();
	if (!new_cmd)
		return (false);
	prev_cmd->next = new_cmd;
	prev_cmd->operator = TOK_PIPE;
	*cmd = new_cmd;
	*current = (*current)->next;
	if (!process_command_tokens(current, *cmd, ctx))
	{
		prev_cmd->next = NULL;
		free_command(new_cmd);
		return (false);
	}
	if (*current && (*current)->type == TOK_PIPE)
		return (create_pipeline(cmd, current, ctx));
	return (true);
}

/**
 * @brief Parses tokens into a command structure
 *
 * @param ctx Context containing environment information
 * @param tokens The tokens array to parse
 * @return t_command* Parsed command structure or NULL if parsing fails
 */
t_command	*command_parse(t_ctx *ctx, t_token *tokens)
{
	t_command	*cmd;
	t_command	*first_cmd;
	t_token		*current;

	cmd = command_new();
	if (!cmd)
		return (NULL);
	first_cmd = cmd;
	current = tokens;
	if (!process_command_tokens(&current, cmd, ctx))
	{
		free_all_commands(first_cmd);
		return (NULL);
	}
	if (current && current->type == TOK_PIPE)
	{
		if (!create_pipeline(&cmd, &current, ctx))
		{
			free_all_commands(first_cmd);
			return (NULL);
		}
	}
	return (first_cmd);
}
