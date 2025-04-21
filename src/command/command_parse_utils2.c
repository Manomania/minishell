/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_parse_utils2.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 18:07:56 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/21 18:09:19 by elagouch         ###   ########.fr       */
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
int	handle_redirection_token(t_command *cmd, t_token *token,
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
	result = command_add_redirection(cmd, token->type, expanded_filename);
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
t_bool	handle_empty_first_arg(t_command *cmd, t_token **current, t_ctx *ctx)
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
 * @brief Processes word token and handles first token case
 *
 * @param cmd Current command being built
 * @param current Current token
 * @param ctx Context containing environment information
 * @param first_arg_processed Flag indicating if first arg was processed
 * @return t_bool true on success, false on failure
 */
t_bool	process_word_token_case(t_command *cmd, t_token **current, t_ctx *ctx,
		t_bool *first_arg_processed)
{
	char	*expanded_value;

	expanded_value = handle_quotes_and_vars(ctx, (*current)->value);
	if (!expanded_value)
		return (false);
	if (!(*first_arg_processed) && expanded_value[0] == '\0')
	{
		free(expanded_value);
		*current = (*current)->next;
		*first_arg_processed = true;
		if (!handle_empty_first_arg(cmd, current, ctx))
			return (false);
		return (true);
	}
	free(expanded_value);
	if (!process_word_token(cmd, *current, ctx))
		return (false);
	*first_arg_processed = true;
	return (true);
}
