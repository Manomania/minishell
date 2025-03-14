/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maximart <maximart@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 14:35:40 by maximart          #+#    #+#             */
/*   Updated: 2025/03/10 14:35:43 by maximart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	advance_parse(t_parse *parse)
{
	if (parse->current && parse->current->type != TOK_EOF)
		parse->current = parse->current->next;
}

int	check_parse(t_parse *parse, t_token_type type)
{
	return (parse->current && parse->current->type == type);
}

int	check_token_type(t_parse *parse, t_token_type type)
{
	return (parse->current && parse->current->type == type);
}

char	*get_token_value(t_parse *parse)
{
	if (!parse->current)
		return (NULL);
	return (parse->current->value);
}

int	consume_parse(t_parse *parse, t_token_type type)
{
	if (check_parse(parse, type))
	{
		advance_parse(parse);
		return (1);
	}
	return (0);
}