/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_tokenize.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 15:28:06 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/14 15:28:15 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
 */
t_token	*tokenize(char *input)
{
	t_lexer	*lexer;
	t_token	*head;

	head = NULL;
	lexer = create_lexer(input);
	if (!lexer)
		return (NULL);
	if (!build_token_list(lexer, &head))
	{
		free(lexer);
		return (NULL);
	}
	free(lexer);
	return (head);
}
