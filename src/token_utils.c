/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maximart <maximart@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 19:19:16 by maximart          #+#    #+#             */
/*   Updated: 2025/03/05 19:19:27 by maximart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


void	free_token(t_token *token)
{
	if (token)
	{
		free(token->value);
		free(token);
	}
}

void	free_all_token(t_token *token)
{
	t_token *current;
	t_token *next;

	current = token;
	while (current)
	{
		next = current->next;
		free_token(current);
		current = next;
	}
}
