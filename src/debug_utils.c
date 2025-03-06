/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 14:36:02 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/06 14:36:09 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_tokens_list(t_token *tokens)
{
	t_token	*current;
	int		token_count;

	current = tokens;
	token_count = 0;
	ft_printf(YELLOW "\n===== TOKEN LIST =====\n" RESET);
	while (current)
	{
		ft_printf(GREEN "Token %d: " RESET, token_count++);
		ft_printf("Type: " YELLOW);
		switch (current->type)
		{
		case TOK_WORD:
			ft_printf("WORD");
			break ;
		case TOK_REDIR_FROM:
			ft_printf("REDIR_FROM");
			break ;
		case TOK_REDIR_TO:
			ft_printf("REDIR_TO");
			break ;
		case TOK_HERE_DOC_FROM:
			ft_printf("HERE_DOC_FROM");
			break ;
		case TOK_HERE_DOC_TO:
			ft_printf("HERE_DOC_TO");
			break ;
		case TOK_PIPE:
			ft_printf("PIPE");
			break ;
		case TOK_ENV:
			ft_printf("ENV");
			break ;
		case TOK_AND:
			ft_printf("AND");
			break ;
		case TOK_OR:
			ft_printf("OR");
			break ;
		case TOK_NEW_LINE:
			ft_printf("NEW_LINE");
			break ;
		case TOK_EOF:
			ft_printf("EOF");
			break ;
		default:
			ft_printf("UNKNOWN");
			break ;
		}
		ft_printf(RESET);
		if (current->value)
			ft_printf(" | Value: " GREEN "%s" RESET, current->value);
		else
			ft_printf(" | Value: " GREEN "NULL" RESET);
		ft_printf("\n");
		current = current->next;
	}
	ft_printf(YELLOW "=====================\n\n" RESET);
}
