/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_parse.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 19:14:26 by maximart          #+#    #+#             */
/*   Updated: 2025/03/12 17:57:47 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Create and initializes a new lexer
 *
 * @param input Input string to be processed
 * @return New lexer pointer or NULL if allocation fails
 * @note Caller must free the returned lexer
 */
t_lexer	*create_lexer(char *input)
{
	t_lexer	*lexer;

	lexer = malloc(sizeof(t_lexer));
	if (!lexer)
		return (NULL);
	lexer->input = input;
	lexer->position = 0;
	lexer->length = (int)ft_strlen(input);
	return (lexer);
}

/**
 * @brief Creates and initializes a new token
 *
 * @param type Token type enumeration
 * @param value String value of the token
 * @return New token pointer or NULL if allocation fails
 * @note Caller must free the returned token
 */
t_token	*create_token(t_token_type type, char *value)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = type;
	token->next = NULL;
	token->value = value;
	return (token);
}

/**
 * @brief Creates and initializes a new command
 *
 * @return New command pointer or NULL if allocation fails
 * @note Caller must free the returned token
 */
t_command	*create_command(void)
{
	t_command	*cmd;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cmd->redirection = NULL;
	cmd->next = NULL;
	cmd->args = NULL;
	cmd->arg_count = 0;
	return (cmd);
}

/**
 * @brief Creates and initializes a new command
 *
 * @param type
 * @param filename
 * @return New command pointer or NULL if allocation fails
 * @note Caller must free the returned token
 */
t_redirection	*create_redirection(t_token_type type, char *filename)
{
	t_redirection	*redirection;

	redirection = malloc(sizeof(t_redirection));
	if (!redirection)
		return (NULL);
	redirection->type = type;
	redirection->next = NULL;
	redirection->filename = ft_strdup(filename);
	if (!redirection->filename)
	{
		free(redirection);
		return (NULL);
	}
	return (redirection);
}

void	init_parse_context(t_parse *parse, t_token *token)
{
	parse->token = token;
	parse->current = token;
}