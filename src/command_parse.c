/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_parse.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 13:46:45 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/08 14:03:42 by elagouch         ###   ########.fr       */
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
 * @brief Parses tokens into a command structure
 *
 * @param ctx Context containing tokens
 * @return t_command* Parsed command structure
 */
t_command	*command_parse(t_ctx *ctx)
{
	t_command	*cmd;
	t_token		*current;

	cmd = command_new();
	if (!cmd)
		return (NULL);
	current = ctx->tokens;
	while (current && current->type != TOK_PIPE)
	{
		if (current->type == TOK_WORD)
		{
			if (!cmd->cmd)
				cmd->cmd = ft_strdup(current->value);
			else
				command_add_argument(cmd, current->value);
		}
		else if (token_is_redirection(current->type))
		{
			handle_redirection(cmd, current, current->next);
			current = current->next;
		}
		if (current)
			current = current->next;
	}
	return (cmd);
}
