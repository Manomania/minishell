/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_parsing.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maximart <maximart@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 19:14:26 by maximart          #+#    #+#             */
/*   Updated: 2025/03/05 19:14:29 by maximart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief			Create and initializes a new lexer
 * @param	input	Input string to be processed
 * @return			New lexer pointer or NULL if allocation fails
 * @note			Caller must free the returned lexer
 */
t_lexer *create_lexer(char *input)
{
	t_lexer *lexer;

	lexer = malloc(sizeof(t_lexer));
	if (!lexer)
		return (NULL);
	lexer->input = input;
	lexer->position = 0;
	lexer->length = (int)ft_strlen(input);
	return (lexer);
}

t_token	*create_token(t_token_type type, char *value)
{
	t_token *token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = type;
	token->next = NULL;
	token->value = value;
	return (token);
}