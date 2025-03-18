/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_tokenize.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 15:28:06 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/17 17:17:54 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "debug.h"
#include "error.h"
#include "memory.h"
#include "minishell.h"

/**
 * @brief Adds a token to the linked list, updating head/current pointers
 */
static void	add_token_to_list(t_token **head, t_token **current, t_token *token)
{
	if (!*head)
	{
		*head = token;
		*current = token;
	}
	else
	{
		(*current)->next = token;
		*current = token;
	}
}

/**
 * @brief Builds the token list from lexer input
 */
static int	build_token_list(t_lexer *lexer, t_token **head)
{
	t_token	*token;
	t_token	*current;

	current = NULL;
	while (1)
	{
		token = next_token_lexer(lexer);
		if (!token)
		{
			free_all_token(*head);
			return (0);
		}
		add_token_to_list(head, &current, token);
		if (token->type == TOK_EOF)
			break ;
	}
	return (1);
}

/**
 * @brief Tokenizes input string into a linked list of tokens
 *
 * @param ctx Context for error handling
 * @param input The input string to tokenize
 * @return Linked list of tokens or NULL on error
 */
t_token	*tokenize(t_ctx *ctx, char *input)
{
	t_lexer	*lexer;
	t_token	*head;

	head = NULL;
	debug_log(DEBUG_INFO, "lexer", "Tokenizing input");
	if (!input)
	{
		error_print(ERROR, "lexer", "NULL input string");
		return (NULL);
	}
	lexer = safe_malloc(ctx, sizeof(t_lexer), "lexer");
	lexer->input = input;
	lexer->position = 0;
	lexer->length = (int)ft_strlen(input);
	if (!build_token_list(lexer, &head))
	{
		error_print(ERROR, "lexer", "Failed to build token list");
		free(lexer);
		return (NULL);
	}
	free(lexer);
	debug_log(DEBUG_INFO, "lexer", "Tokenization complete");
	return (head);
}
