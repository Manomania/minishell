/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_token_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 13:44:05 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/17 13:44:49 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Handles redirection tokens during tokenization
 *
 * This function processes redirection operators (<, >, <<, >>)
 * and creates the appropriate token.
 *
 * @param lexer Lexer structure containing input and position
 * @param current Current character being processed
 * @return Token representing the redirection operator
 */
t_token	*handle_redirection(t_lexer *lexer, char current)
{
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
	else if (current == '>')
	{
		advance_lexer(lexer);
		if (get_lexer(lexer) == '>')
		{
			advance_lexer(lexer);
			return (create_token(TOK_HERE_DOC_TO, ft_strdup(">>")));
		}
		return (create_token(TOK_REDIR_TO, ft_strdup(">")));
	}
	return (NULL);
}

/**
 * @brief Handles pipe and logical operators during tokenization
 *
 * This function processes pipe (|) and logical operators (&&, ||)
 * and creates the appropriate token.
 *
 * @param lexer Lexer structure containing input and position
 * @param current Current character being processed
 * @return Token representing the operator
 */
t_token	*handle_operators(t_lexer *lexer, char current)
{
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
	else if (current == '&')
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
	return (NULL);
}

/**
 * @brief Handles special characters during tokenization
 *
 * This function processes special characters like quotes and variables
 * and creates the appropriate token.
 *
 * @param lexer Lexer structure containing input and position
 * @param current Current character being processed
 * @return Token representing the processed text
 */
t_token	*handle_special_chars(t_lexer *lexer, char current)
{
	char	*word;

	if (current == '$')
	{
		advance_lexer(lexer);
		word = read_word_lexer(lexer);
		if (word)
			return (create_token(TOK_ENV, word));
		return (create_token(TOK_ENV, ft_strdup("")));
	}
	else if (current == '\n')
	{
		advance_lexer(lexer);
		return (create_token(TOK_NEW_LINE, ft_strdup("\n")));
	}
	else if (current == '\0')
	{
		return (create_token(TOK_EOF, NULL));
	}
	return (NULL);
}
