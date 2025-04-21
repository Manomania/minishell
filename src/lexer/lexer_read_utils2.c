/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_read_utils2.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 16:01:00 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/21 16:02:34 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Sets the quote flags in lexer structure
 *
 * @param lexer Pointer to lexer structure
 * @param quote_char Quote character (' or ")
 */
void	set_quote_flags(t_lexer *lexer, char quote_char)
{
	if (quote_char == '"')
		lexer->quote.in_double_quote = 1;
	else if (quote_char == '\'')
		lexer->quote.in_single_quote = 1;
}

/**
 * @brief Create a string from the quoted content
 *
 * @param lexer Pointer to lexer structure
 * @param start Start position of the content
 * @param end End position of the content
 * @return char* Allocated string containing content or NULL on error
 */
char	*create_quoted_content(t_lexer *lexer, int start, int end)
{
	char	*content;

	if (start == end)
		return (ft_strdup(""));
	content = malloc((end - start + 1));
	if (!content)
		return (NULL);
	ft_strlcpy(content, lexer->input + start, (size_t)(end - start + 1));
	content[end - start] = '\0';
	return (content);
}
