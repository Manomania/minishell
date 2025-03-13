/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_token.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maximart <maximart@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 14:31:33 by maximart          #+#    #+#             */
/*   Updated: 2025/03/10 14:31:43 by maximart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
	char	*word;

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
	if (get_lexer(lexer) == '$')
	{
		token = handle_env_token(lexer);
		if (token)
			return (token);
	}
	word = read_complex_word(lexer);
	if (!word)
		return (NULL);
	if (word[0] == '\0' && get_lexer(lexer) == '$')
	{
		free(word);
		return (handle_env_token(lexer));
	}
	return (create_token(TOK_WORD, word));
}

void	free_token(t_token *token)
{
	if (token)
	{
		free(token->value);
		free(token);
	}
}

void	free_all_token(t_token *token)
{
	t_token *current;
	t_token *next;

	current = token;
	while (current)
	{
		next = current->next;
		free_token(current);
		current = next;
	}
}