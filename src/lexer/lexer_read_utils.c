/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_read_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maximart <maximart@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/19 10:43:47 by maximart          #+#    #+#             */
/*   Updated: 2025/03/19 10:43:50 by maximart         ###   ########.fr       */
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
 * @brief Handle dollar sign during complex word reading
 *
 * @param lexer Pointer to lexer structure
 * @param result Current result buffer
 * @return Updated result buffer with position at dollar sign
 */
char	*handle_dollar_sign(char *result)
{
	if (!result)
		result = ft_strdup("");
	return (result);
}

/**
 * @brief Handle word part during complex word reading
 *
 * @param lexer Pointer to lexer structure
 * @param result Current result buffer
 * @return Updated result buffer or NULL on error
 */
char	*handle_word_part(t_lexer *lexer, char *result)
{
	char	*part;

	part = read_word_lexer(lexer);
	if (!part)
	{
		return (ft_strdup(""));
	}
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
char	*handle_quoted_part(t_lexer *lexer, char *result, char quote_char)
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
