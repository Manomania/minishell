/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_pipeline.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 14:39:38 by maximart          #+#    #+#             */
/*   Updated: 2025/03/18 11:48:16 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Parses a pipeline with environment variable expansion
 *
 * @param parse Parser context
 * @param ctx Shell context
 * @return Command structure or NULL on error
 */
t_command	*parse_pipeline(t_ctx *ctx, t_parse *parse)
{
	t_command	*first_cmd;
	t_command	*current_cmd;

	first_cmd = parse_command(parse, ctx);
	if (!first_cmd)
		return (NULL);
	current_cmd = first_cmd;
	while (parse->current && (parse->current->type == TOK_PIPE))
	{
		current_cmd->operator = parse->current->type;
		advance_parse(parse);
		current_cmd->next = parse_command(parse, ctx);
		if (!current_cmd->next)
		{
			free_all_commands(first_cmd);
			return (NULL);
		}
		current_cmd = current_cmd->next;
	}
	return (first_cmd);
}

static void	connect_commands(t_command *left_cmd, t_command *right_cmd,
		t_token_type op_type)
{
	t_command	*last_cmd;

	last_cmd = left_cmd;
	while (last_cmd->next)
		last_cmd = last_cmd->next;
	last_cmd->next = right_cmd;
	last_cmd->operator = op_type;
}

/**
 * @brief Parses a command sequence with environment variable expansion
 *
 * @param parse Parser context
 * @param ctx Shell context
 * @return Command structure or NULL on error
 */
t_command	*parse_command_sequence(t_ctx *ctx, t_parse *parse)
{
	t_command		*left_cmd;
	t_command		*right_cmd;
	t_token_type	op_type;

	left_cmd = parse_pipeline(ctx, parse);
	if (!left_cmd)
		return (NULL);
	op_type = parse->current->type;
	advance_parse(parse);
	right_cmd = parse_command_sequence(ctx, parse);
	if (!right_cmd)
	{
		free_all_commands(left_cmd);
		return (NULL);
	}
	connect_commands(left_cmd, right_cmd, op_type);
	return (left_cmd);
}
