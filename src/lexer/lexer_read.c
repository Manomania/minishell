/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_read.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 14:27:07 by maximart          #+#    #+#             */
/*   Updated: 2025/04/25 13:27:14 by elagouch         ###   ########.fr       */
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
			"minishell: syntax error near unexpected unclosed quote\n");
		return (NULL);
	}
	end = lexer->position;
	set_quote_flags(lexer, quote_char);
	advance_lexer(lexer);
	content = create_quoted_content(lexer, start, end);
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
	char	*part;

	if (quote_char != 0)
	{
		part = read_quoted_string_lexer(lexer, quote_char);
		if (!part)
		{
			free(result);
			return (NULL);
		}
		new_result = join_and_free(result, part);
		free(part);
		return (new_result);
	}
	else
		return (handle_word_part(lexer, result));
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
		if (get_lexer(lexer) == '$' && lexer->position + 1 < lexer->length
			&& (lexer->input[lexer->position + 1] == '"'
				|| lexer->input[lexer->position + 1] == '\''))
		{
			if (result)
				return (result);
			return (handle_dollar_quotes(lexer));
		}
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
