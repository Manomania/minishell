/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_token.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 14:31:33 by maximart          #+#    #+#             */
/*   Updated: 2025/03/17 13:43:58 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Extracts the next token from the lexer
 *
 * This function reads the next token from the input stream,
 * handling various token types appropriately.
 *
 * @param lexer Lexer structure containing input and position
 * @return Next token or NULL on error
 */
t_token	*next_token_lexer(t_lexer *lexer)
{
	char	*word;
	char	current;
	t_token	*token;

	skip_whitespace_lexer(lexer);
	current = get_lexer(lexer);
	token = handle_redirection(lexer, current);
	if (token)
		return (token);
	token = handle_operators(lexer, current);
	if (token)
		return (token);
	token = handle_special_chars(lexer, current);
	if (token)
		return (token);
	word = read_complex_word(lexer);
	if (!word)
		return (NULL);
	return (create_token(TOK_WORD, word));
}

/**
 * @brief Frees memory associated with a token
 *
 * This function releases the memory allocated for a token,
 * including its value string.
 *
 * @param token Token to free
 */
void	free_token(t_token *token)
{
	if (token)
	{
		free(token->value);
		free(token);
	}
}

/**
 * @brief Frees all tokens in a linked list
 *
 * This function releases the memory allocated for all tokens
 * in a linked list, starting from the given token.
 *
 * @param token First token in the list
 */
void	free_all_token(t_token *token)
{
	t_token	*current;
	t_token	*next;

	current = token;
	while (current)
	{
		next = current->next;
		free_token(current);
		current = next;
	}
}
