/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_command.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maximart <maximart@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 14:37:01 by maximart          #+#    #+#             */
/*   Updated: 2025/03/10 14:37:13 by maximart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	add_argument(t_command *cmd, char *value)
{
	char	**new_args;
	int		i;

	if (!cmd || !value)
		return (0);
	new_args = malloc(sizeof(char *) * (cmd->arg_count + 2));
	if (!new_args)
		return (0);
	i = 0;
	while (i < cmd->arg_count)
	{
		new_args[i] = cmd->args[i];
		i++;
	}
	new_args[i] = ft_strdup(value);
	if (!new_args[i])
	{
		free(new_args);
		return (0);
	}
	new_args[i + 1] = NULL;
	if (cmd->args)
		free(cmd->args);
	cmd->args = new_args;
	cmd->arg_count++;
	return (1);
}

void	add_redirection(t_command *cmd, t_redirection *redirection)
{
	t_redirection	*current;

	if (!cmd->redirection)
	{
		cmd->redirection = redirection;
		return ;
	}
	current = cmd->redirection;
	while (current->next)
		current = current->next;
	current->next = current;
}

/**
 * @brief Parses a redirection with environment variable expansion
 *
 * @param parse Parser context
 * @param cmd Command structure
 * @param ctx Shell context
 * @return 1 if successful, 0 on error
 */
int	parse_redirection(t_parse *parse, t_command *cmd, t_ctx *ctx)
{
	t_token_type	type;
	t_redirection	*redirection;
	char			*expanded;

	type = parse->current->type;
	advance_parse(parse);
	if (!parse->current || parse->current->type != TOK_WORD)
	{
		ft_printf(RED"Error: Expected filename after redirection\n"RESET);
		return (0);
	}
	if (type == TOK_HERE_DOC_FROM)
		redirection = create_redirection(type, parse->current->value);
	else
	{
		expanded = handle_quotes_and_vars(ctx, parse->current->value);
		redirection = create_redirection(type, expanded);
		free(expanded);
	}
	if (!redirection)
		return (0);
	add_redirection(cmd, redirection);
	advance_parse(parse);
	return (1);
}

/**
 * @brief Process token value with environment variable expansion
 *
 * @param ctx Shell context
 * @param token_value Token value to process
 * @return Processed value with environment variables expanded
 */
char	*process_token(t_ctx *ctx, char *token_value)
{
	char	*expanded;

	expanded = handle_quotes_and_vars(ctx, token_value);
	return (expanded);
}

/**
 * @brief Parses a command with environment variable expansion
 *
 * @param parse Parser context
 * @param ctx Shell context
 * @return Command structure or NULL on error
 */
t_command	*parse_command(t_parse *parse, t_ctx *ctx)
{
	t_command	*cmd;
	char		*expanded;

	cmd = create_command();
	if (!cmd)
		return (NULL);
	while (parse->current && parse->current->type != TOK_PIPE
		&& parse->current->type != TOK_AND && parse->current->type != TOK_OR
		&& parse->current->type != TOK_EOF)
	{
		if (parse->current->type == TOK_WORD)
		{
			expanded = handle_quotes_and_vars(ctx, parse->current->value);
			if (!add_argument(cmd, expanded))
			{
				free(expanded);
				free_command(cmd);
				return (NULL);
			}
			free(expanded);
			advance_parse(parse);
		}
		else if (parse->current->type == TOK_ENV)
		{
			expanded = expand_var(ctx, parse->current->value);
			if (!add_argument(cmd, expanded))
			{
				free(expanded);
				free_command(cmd);
				return (NULL);
			}
			free(expanded);
			advance_parse(parse);
		}
		else if (parse->current->type == TOK_REDIR_FROM
			|| parse->current->type == TOK_REDIR_TO
			|| parse->current->type == TOK_HERE_DOC_FROM
			|| parse->current->type == TOK_HERE_DOC_TO)
		{
			if (!parse_redirection(parse, cmd, ctx))
			{
				free_command(cmd);
				return (NULL);
			}
		}
		else
			advance_parse(parse);
	}
	return (cmd);
}

/**
 * @brief Parses a token list with environment variable expansion
 *
 * @param token Token list
 * @param ctx Shell context
 * @return Command structure or NULL on error
 */
t_command	*parse_token(t_token *token, t_ctx *ctx)
{
	t_parse		parse;
	t_command	*cmd;

	init_parse_context(&parse, token);
	if (parse.current->type == TOK_AND ||
	parse.current->type == TOK_OR ||
	parse.current->type == TOK_PIPE)
	{
		ft_printf(RED"Error: Unexpected token at start of command\n"RESET);
		return (NULL);
	}
	cmd = parse_command_sequence(&parse, ctx);
	if (!cmd)
		return (NULL);
	if (parse.current->type != TOK_EOF)
	{
		ft_printf(RED"Error:\nUnexpected token\n"RESET);
		free_all_commands(cmd);
		return (NULL);
	}
	return (cmd);
}