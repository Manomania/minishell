/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_read.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maximart <maximart@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 14:27:07 by maximart          #+#    #+#             */
/*   Updated: 2025/03/10 14:27:21 by maximart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Reads a quoted string from the lexer input stream
 *
 * @param lexer Pointer to lexer structure
 * @param quote_char The quote delimiter character
 * @return Newly allocated string without quotes or NULL on error
 * @note Caller must free the returned string
 */
char	*read_quoted_string_lexer(t_lexer *lexer, char quote_char)
{
	int		length;
	int		start;
	int		end;
	char	*content;

	start = lexer->position + 1;
	advance_lexer(lexer);
	while (get_lexer(lexer) != '\0' && get_lexer(lexer) != quote_char)
		advance_lexer(lexer);
	if (get_lexer(lexer) == '\0')
	{
		ft_printf(RED"Error:\nUnclosed quote\n"RESET);
		return (NULL);
	}
	end = lexer->position;
	advance_lexer(lexer);
	length = end - start;
	content = malloc((unsigned long)length + 1);
	if (!content)
		return (NULL);
	ft_strlcpy(content, lexer->input + start, (size_t)length + 1);
	content[length] = '\0';
	return (content);
}

/**
 * @brief Extract a word from lexer input
 *
 * @param lexer Pointer to lexer structure
 * @return Newly allocated string containing the word or NULL on error
 * @note Caller must free the returned string
 */
char	*read_word_lexer(t_lexer *lexer)
{
	int		start;
	int		length;
	char	*word;

	start = lexer->position;
	while (get_lexer(lexer) != '\0' && get_lexer(lexer) != ' '
		&& get_lexer(lexer) != '\t' && get_lexer(lexer) != '\n'
		&& get_lexer(lexer) != '|' && get_lexer(lexer) != '<'
		&& get_lexer(lexer) != '>' && get_lexer(lexer) != '&'
		&& get_lexer(lexer) != '"' && get_lexer(lexer) != '$'
		&& get_lexer(lexer) != '\'')
		advance_lexer(lexer);
	length = lexer->position - start;
	word = malloc((unsigned long)length + 1);
	if (!word)
		return (NULL);
	ft_strlcpy(word, lexer->input + start, (size_t)length + 1);
	word[length] = '\0';
	return (word);
}

t_token	*tokenize(char *input)
{
	t_lexer	*lexer;
	t_token	*token;
	t_token	*head;
	t_token	*current;

	lexer = create_lexer(input);
	if (!lexer)
		return (NULL);
	head = NULL;
	current = NULL;
	while (1)
	{
		token = next_token_lexer(lexer);
		if (!head)
		{
			head = token;
			current = token;
		}
		else
		{
			current->next = token;
			current = token;
		}
		if (token->type == TOK_EOF)
			break ;
	}
	free(lexer);
	return (head);
}