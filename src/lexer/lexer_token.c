/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_token.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 14:31:33 by maximart          #+#    #+#             */
/*   Updated: 2025/04/07 19:16:10 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Handles a word token in the lexer
 *
 * @param lexer Current lexer state
 * @return t_token* Created word token or NULL on error
 */
static t_token	*handle_word_token(t_lexer *lexer)
{
	char	*word;

	word = read_complex_word(lexer);
	if (!word)
		return (NULL);
	return (create_token(TOK_WORD, word));
}

/**
 * @brief Extracts the next token from the lexer
 *
 * @param lexer Pointer to lexer structure
 * @return Next token or NULL on error
 * @note Caller must free the returned token
 */
t_token	*next_token_lexer(t_lexer *lexer)
{
	t_token	*token;

	skip_whitespace_lexer(lexer);
	token = handle_basics_token(lexer);
	if (token)
		return (token);
	token = handle_pipe_and_token(lexer);
	if (token)
		return (token);
	token = handle_redir_from_and_to_token(lexer);
	if (token)
		return (token);
	token = handle_word_token(lexer);
	if (token)
		return (token);
	return (NULL);
}

/**
 * @brief Frees a single token
 *
 * @param token Token to free
 */
static void	free_token(t_token *token)
{
	if (token)
	{
		if (token->value)
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
