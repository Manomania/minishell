/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_parse.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 17:42:48 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/08 17:48:22 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Parses tokens into a pipeline structure
 *
 * @param ctx Context containing tokens
 * @return t_pipeline* Pipeline structure or NULL on error
 */
t_pipeline	*pipeline_parse(t_ctx *ctx)
{
	t_pipeline	*pipeline;
	t_command	*current_cmd;
	t_token		*current_token;

	pipeline = pipeline_new();
	if (!pipeline)
		return (NULL);
	current_token = ctx->tokens;
	while (current_token && current_token->type != TOK_EOF)
	{
		current_cmd = command_parse(ctx);
		if (!current_cmd)
			break ;
		pipeline_add_command(pipeline, current_cmd);
		if (current_token && current_token->type == TOK_PIPE)
			current_token = current_token->next;
		else
			break ;
	}
	if (!pipeline->commands)
	{
		free(pipeline);
		return (NULL);
	}
	return (pipeline);
}
