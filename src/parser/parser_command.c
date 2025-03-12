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

int	parse_redirection(t_parse *parse, t_command *cmd)
{
	t_token_type	type;
	t_redirection	*redirection;

	type = parse->current->type;
	advance_parse(parse);
	if (!parse->current || parse->current->type != TOK_WORD)
	{
		ft_printf(RED"Error: Expected filename after redirection\n"RESET);
		return (0);
	}
	redirection = create_redirection(type, parse->current->value);
	if (!redirection)
		return (0);
	add_redirection(cmd, redirection);
	advance_parse(parse);
	return (1);
}

t_command	*parse_command(t_parse *parse)
{
	t_command	*cmd;

	cmd = create_command();
	if (!cmd)
		return (NULL);
	while (parse->current && parse->current->type != TOK_PIPE
		&& parse->current->type != TOK_AND && parse->current->type != TOK_OR
		&& parse->current->type != TOK_EOF)
	{
		if (parse->current->type == TOK_WORD)
		{
			if (!add_argument(cmd, parse->current->value))
			{
				free(cmd);
				return (NULL);
			}
			advance_parse(parse);
		}
		else if (parse->current->type == TOK_REDIR_FROM
			|| parse->current->type == TOK_REDIR_TO
			|| parse->current->type == TOK_HERE_DOC_FROM
			|| parse->current->type == TOK_HERE_DOC_TO)
		{
			if (!parse_redirection(parse, cmd))
				return (NULL);
		}
		else
			advance_parse(parse);
	}
	return (cmd);
}

t_command	*parse_token(t_token *token)
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
	cmd = parse_command_sequence(&parse);
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