/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_token.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 14:31:33 by maximart          #+#    #+#             */
/*   Updated: 2025/03/14 15:30:27 by elagouch         ###   ########.fr       */
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
	char	*word;
	char	current;

	skip_whitespace_lexer(lexer);
	current = get_lexer(lexer);
	if (current == '\0')
		return (create_token(TOK_EOF, NULL));
	if (current == '\n')
	{
		advance_lexer(lexer);
		return (create_token(TOK_NEW_LINE, ft_strdup("\n")));
	}
	if (current == '|')
	{
		advance_lexer(lexer);
		if (get_lexer(lexer) == '|')
		{
			advance_lexer(lexer);
			return (create_token(TOK_OR, ft_strdup("||")));
		}
		return (create_token(TOK_PIPE, ft_strdup("|")));
	}
	if (current == '&')
	{
		advance_lexer(lexer);
		if (get_lexer(lexer) == '&')
		{
			advance_lexer(lexer);
			return (create_token(TOK_AND, ft_strdup("&&")));
		}
		ft_printf(RED "error:\nUnexpected '&'\n" RESET);
		return (create_token(TOK_EOF, NULL));
	}
	if (current == '<')
	{
		advance_lexer(lexer);
		if (get_lexer(lexer) == '<')
		{
			advance_lexer(lexer);
			return (create_token(TOK_HERE_DOC_FROM, ft_strdup("<<")));
		}
		return (create_token(TOK_REDIR_FROM, ft_strdup("<")));
	}
	if (current == '>')
	{
		advance_lexer(lexer);
		if (get_lexer(lexer) == '>')
		{
			advance_lexer(lexer);
			return (create_token(TOK_HERE_DOC_TO, ft_strdup(">>")));
		}
		return (create_token(TOK_REDIR_TO, ft_strdup(">")));
	}
	if (current == '$')
	{
		advance_lexer(lexer);
		word = read_word_lexer(lexer);
		if (word)
			return (create_token(TOK_ENV, word));
		return (create_token(TOK_ENV, ft_strdup("")));
	}
	word = read_complex_word(lexer);
	if (!word)
		return (NULL);
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
