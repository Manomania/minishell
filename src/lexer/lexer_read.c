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
		ft_printf(RED"Error:\nUnclosed quote\n"RESET);
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
 * @brief Reads a complex word that might contain environment variables
 *
 * @param lexer Pointer to lexer structure
 * @return Newly allocated string containing the word or NULL on error
 * @note Caller must free the returned string
 */
char	*read_complex_word(t_lexer *lexer)
{
	char	*result;
	char	*part;
	char	quote_char;
	int		dollar_pos;

	result = NULL;
	while (get_lexer(lexer) != '\0' && get_lexer(lexer) != ' '
		&& get_lexer(lexer) != '\t' && get_lexer(lexer) != '\n'
		&& get_lexer(lexer) != '|' && get_lexer(lexer) != '<'
		&& get_lexer(lexer) != '>' && get_lexer(lexer) != '&')
	{
		if (get_lexer(lexer) == '"' || get_lexer(lexer) == '\'')
		{
			quote_char = get_lexer(lexer);
			part = read_quoted_string_lexer(lexer, quote_char);
			if (!part)
			{
				free(result);
				return (NULL);
			}
			result = join_and_free(result, part);
			free(part);
		}
		else if (get_lexer(lexer) == '$')
		{
			dollar_pos = lexer->position;
			if (!result)
				result = ft_strdup("");
			break;
		}
		else
		{
			part = read_word_lexer(lexer);
			result = join_and_free(result, part);
			free(part);
		}
	}

	if (!result)
		return (ft_strdup(""));

	return (result);
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
		if (!token)
		{
			free_all_token(head);
			free(lexer);
			return (NULL);
		}
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