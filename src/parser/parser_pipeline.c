/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_pipeline.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maximart <maximart@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 14:39:38 by maximart          #+#    #+#             */
/*   Updated: 2025/03/10 14:39:41 by maximart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_command	*parse_pipeline(t_parse *parse)
{
	t_command	*first_cmd;
	t_command	*current_cmd;

	first_cmd = parse_command(parse);
	if (!first_cmd)
		return (NULL);
	current_cmd = first_cmd;
	while (parse->current && (parse->current->type == TOK_PIPE
			|| parse->current->type == TOK_OR
			|| parse->current->type == TOK_AND))
	{
		current_cmd->operator= parse->current->type;
		advance_parse(parse);
		current_cmd->next = parse_command(parse);
		if (!current_cmd->next)
		{
			free_all_commands(first_cmd);
			return (NULL);
		}
		current_cmd = current_cmd->next;
	}
	return (first_cmd);
}

t_command	*parse_command_sequence(t_parse *parse)
{
	t_command		*left_cmd;
	t_command		*right_cmd;
	t_token_type	op_type;

	left_cmd = parse_pipeline(parse);
	if (!left_cmd)
		return (NULL);
	if (parse->current->type != TOK_AND && parse->current->type != TOK_OR)
		return (left_cmd);
	op_type = parse->current->type;
	advance_parse(parse);
	right_cmd = parse_command_sequence(parse);
	if (!right_cmd)
	{
		free_all_commands(left_cmd);
		return (NULL);
	}
	// Connect left and right commands based on operator
	connect_commands(left_cmd, right_cmd, op_type);
	return (left_cmd);
}

void	connect_commands(t_command *left_cmd, t_command *right_cmd,
		t_token_type op_type)
{
	t_command *last_cmd;

	last_cmd = left_cmd;
	while (last_cmd->next)
		last_cmd = last_cmd->next;

	last_cmd->next = right_cmd;
	last_cmd->operator= op_type;
}