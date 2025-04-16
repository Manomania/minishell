/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_read.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 14:27:07 by maximart          #+#    #+#             */
/*   Updated: 2025/04/16 15:31:43 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"

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
		&& get_lexer(lexer) != '"' && get_lexer(lexer) != '\'')
		advance_lexer(lexer);
	length = lexer->position - start;
	if (length == 0)
		return (ft_strdup(""));
	word = malloc((unsigned long)length + 1);
	if (!word)
		return (NULL);
	ft_strlcpy(word, lexer->input + start, (size_t)length + 1);
	word[length] = '\0';
	return (word);
}

/**
 * @brief Extract a quoted string from lexer input
 *
 * @param lexer Pointer to lexer structure
 * @param quote_char Quote character (' or ")
 * @return Newly allocated string containing the quoted content or NULL on error
 * @note Caller must free the returned string
 */
char	*read_quoted_string_lexer(t_lexer *lexer, char quote_char)
{
	int		start;
	int		end;
	char	*content;

	start = lexer->position + 1;
	advance_lexer(lexer);
	while (get_lexer(lexer) != '\0' && get_lexer(lexer) != quote_char
		&& lexer->position < lexer->length)
		advance_lexer(lexer);
	if (lexer->position >= lexer->length)
	{
		ft_printf_fd(STDERR_FILENO,
			"syntax error near unexpected unclosed quote\n");
		return (NULL);
	}
	end = lexer->position;
	if (quote_char == '"')
		lexer->quote.in_double_quote = 1;
	else if (quote_char == '\'')
		lexer->quote.in_single_quote = 1;
	advance_lexer(lexer);
	if (start == end)
		return (ft_strdup(""));
	content = malloc((end - start + 1));
	if (!content)
		return (NULL);
	ft_strlcpy(content, lexer->input + start, (size_t)(end - start + 1));
	content[end - start] = '\0';
	return (content);
}

/**
 * @brief Checks if the current character is part of a complex word
 *
 * @param lexer Current lexer state
 * @return int 1 if character is part of a complex word, 0 otherwise
 */
static int	peek_lexer(t_lexer *lexer)
{
	char	current;

	current = get_lexer(lexer);
	return (current != '\0' && current != ' ' && current != '\t'
		&& current != '\n' && current != '|' && current != '<' && current != '>'
		&& current != '&');
}

/**
 * @brief Handles a single part of a complex word
 *
 * @param lexer Current lexer state
 * @param result Current result string
 * @param quote_char Quote character if in quoted mode, 0 otherwise
 * @return char* Updated result string or NULL on error
 */
static char	*handle_word_part_by_type(t_lexer *lexer, char *result,
		char quote_char)
{
	char	*new_result;

	if (quote_char != 0)
		new_result = handle_quoted_part(lexer, result, quote_char);
	else
		new_result = handle_word_part(lexer, result);
	return (new_result);
}

/**
 * @brief Reads a complex word that might contain various elements
 *
 * @param lexer Pointer to lexer structure
 * @return Newly allocated string containing the word or NULL on error
 */
char	*read_complex_word(t_lexer *lexer)
{
	char	*result;
	char	quote_char;

	result = NULL;
	while (peek_lexer(lexer))
	{
		quote_char = 0;
		if (get_lexer(lexer) == '"' || get_lexer(lexer) == '\'')
			quote_char = get_lexer(lexer);
		result = handle_word_part_by_type(lexer, result, quote_char);
		if (!result)
			return (NULL);
	}
	if (!result)
		return (ft_strdup(""));
	return (result);
}
