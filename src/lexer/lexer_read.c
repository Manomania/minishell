/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_read.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 14:27:07 by maximart          #+#    #+#             */
/*   Updated: 2025/03/18 11:51:57 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Joins two strings and frees the first one
 *
 * @param s1 First string (will be freed)
 * @param s2 Second string
 * @return Newly allocated joined string or NULL on error
 */
char	*join_and_free(char *s1, char *s2)
{
	char	*result;

	if (!s1)
		return (ft_strdup(s2));
	result = ft_strjoin(s1, s2);
	free(s1);
	return (result);
}

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
	int		start;
	int		end;
	char	*content;

	start = lexer->position + 1;
	advance_lexer(lexer);
	while (get_lexer(lexer) != '\0' && get_lexer(lexer) != quote_char)
		advance_lexer(lexer);
	if (get_lexer(lexer) == '\0')
	{
		ft_printf(RED "error:\nUnclosed quote\n" RESET);
		return (NULL);
	}
	end = lexer->position;
	advance_lexer(lexer);
	content = malloc((unsigned long)(end - start + 1));
	if (!content)
		return (NULL);
	ft_strlcpy(content, lexer->input + start, (size_t)(end - start + 1));
	content[end - start] = '\0';
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
 * @brief Handle word part during complex word reading
 *
 * @param lexer Pointer to lexer structure
 * @param result Current result buffer
 * @return Updated result buffer or NULL on error
 */
static char	*handle_word_part(t_lexer *lexer, char *result)
{
	char	*part;

	part = read_word_lexer(lexer);
	result = join_and_free(result, part);
	free(part);
	return (result);
}

/**
 * @brief Handle quoted part during complex word reading
 *
 * @param lexer Pointer to lexer structure
 * @param result Current result buffer
 * @param quote_char Quote character (single or double)
 * @return Updated result buffer or NULL on error
 */
static char	*handle_quoted_part(t_lexer *lexer, char *result, char quote_char)
{
	char	*part;

	part = read_quoted_string_lexer(lexer, quote_char);
	if (!part)
	{
		free(result);
		return (NULL);
	}
	result = join_and_free(result, part);
	free(part);
	return (result);
}

/**
 * @brief Handle dollar sign during complex word reading
 *
 * @param lexer Pointer to lexer structure
 * @param result Current result buffer
 * @return Updated result buffer with position at dollar sign
 */
static char	*handle_dollar_sign(char *result)
{
	if (!result)
		result = ft_strdup("");
	return (result);
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
	while (get_lexer(lexer) != '\0' && get_lexer(lexer) != ' '
		&& get_lexer(lexer) != '\t' && get_lexer(lexer) != '\n'
		&& get_lexer(lexer) != '|' && get_lexer(lexer) != '<'
		&& get_lexer(lexer) != '>' && get_lexer(lexer) != '&')
	{
		if (get_lexer(lexer) == '"' || get_lexer(lexer) == '\'')
		{
			quote_char = get_lexer(lexer);
			result = handle_quoted_part(lexer, result, quote_char);
			if (!result)
				return (NULL);
		}
		else if (get_lexer(lexer) == '$')
		{
			result = handle_dollar_sign(result);
			break ;
		}
		else
		{
			result = handle_word_part(lexer, result);
		}
	}
	if (!result)
		return (ft_strdup(""));
	return (result);
}
