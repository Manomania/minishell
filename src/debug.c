/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maximart <maximart@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 19:20:06 by maximart          #+#    #+#             */
/*   Updated: 2025/03/05 19:20:09 by maximart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void print_tokens(t_token *tokens) {
	t_token *current = tokens;
	while (current) {
		printf("Token: ");
		switch (current->type) {
		case TOK_WORD: printf("TOK_WORD"); break;
		case TOK_REDIR_FROM: printf("TOK_REDIR_FROM"); break;
		case TOK_REDIR_TO: printf("TOK_REDIR_TO"); break;
		case TOK_HERE_DOC_FROM: printf("TOK_HERE_DOC_FROM"); break;
		case TOK_HERE_DOC_TO: printf("TOK_HERE_DOC_TO"); break;
		case TOK_PIPE: printf("TOK_PIPE"); break;
		case TOK_ENV: printf("TOK_ENV"); break;
		case TOK_AND: printf("TOK_AND"); break;
		case TOK_OR: printf("TOK_OR"); break;
		case TOK_NEW_LINE: printf("TOK_NEW_LINE"); break;
		case TOK_EOF: printf("TOK_EOF"); break;
		default: printf("UNKNOWN"); break;
		}
		if (current->value) {
			printf(" \"%s\"", current->value);
		}
		printf("\n");
		current = current->next;
	}
}