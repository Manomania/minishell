/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_line.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 10:59:00 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/05 11:30:28 by elagouch         ###   ########.fr       */
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

/**
 * @brief Extract a word from lexer input
 *
 * @param lexer Pointer to lexer structure
 * @return Newly allocated string containing the word or NULL on error
 * @note Caller must free the returned string
 */
t_token	*next_token_lexer(t_lexer *lexer)
{
	char	*word;
	char	*content;
	char	current;
	char	quote;

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
		ft_printf(RED"Error:\nUnexpected '&'\n");
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
	}
	if (current == '"' || current == '\'')
	{
		quote = current;
		content = read_quoted_string_lexer(lexer, quote);
		if (!content)
			return (create_token(TOK_EOF, NULL));
		return (create_token(TOK_WORD, content));
	}
	word = read_word_lexer(lexer);
	return (create_token(TOK_WORD, word));
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

// // t_token	*parse_line(t_ctx ctx, char *line)
// // {
// //
// // }
//
// /*
// // Redirections
// <
// >
//
// // Here_doc
// <<
// >>
//
// // Pipes
// |
//
// // Env
// $ENV
// $?
//
// // Logical operations
// &&
// ||
//
// // Wildcards (no glob)
// *
// hello/*
// hello*
//
// // Le reste: tokens de programme
// echo hello
// echo "hello|"
//
// // Example
// ```sh
// bash-5.1$ echo "hello 'world "shrek" swamp' bitch"
// hello 'world shrek swamp' bitch
// bash-5.1$ echo "hello 'world "sh|rek" swamp' bitch"
// bash: rek swamp' bitch: command not found
// bash-5.1$ echo "hello 'worl|d "shrek" swamp' bitch"
// hello 'worl|d shrek swamp' bitch
// ```
//
// // La regle d'or:
// - Quand on rentre dans un simple/double quote, on attend la fin
// du simple/double quote, c'est tout.
// */
