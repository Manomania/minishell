/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_command.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 14:37:01 by maximart          #+#    #+#             */
/*   Updated: 2025/03/28 11:23:27 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"

/**
 * @brief Adds a redirection to a command
 *
 * This function adds a new redirection to a command's redirection list.
 *
 * @param cmd Command to add the redirection to
 * @param redirection Redirection to add
 */
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
		(void)error(NULL, "redir", ERR_VLD_REDIR_FILENAME_EMPTY);
		return (0);
	}
	if (type == TOK_HERE_DOC_FROM)
		redirection = create_redirection(type, parse->current->value);
	else
	{
		expanded = handle_quotes_and_vars(ctx, parse->current);
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
 * @brief Handles token expansion and adds to command arguments
 *
 * @param cmd Command structure to update
 * @param ctx Shell context
 * @param parse Parser context
 * @return int 1 on success, 0 on failure
 */
static int	handle_token_expansion(t_command *cmd, t_ctx *ctx, t_parse *parse)
{
	char	*expanded;

	if (parse->current->type == TOK_WORD)
		expanded = handle_quotes_and_vars(ctx, parse->current);
	else
		expanded = expand_var(ctx, parse->current->value);
	if (!add_argument(cmd, expanded))
	{
		free(expanded);
		return (0);
	}
	free(expanded);
	advance_parse(parse);
	return (1);
}

/**
 * @brief Processes a single token in the command parsing
 *
 * @param cmd Command structure to update
 * @param ctx Shell context
 * @param parse Parser context
 * @return int 1 on success, 0 on failure
 */
static int	process_command_token(t_command *cmd, t_ctx *ctx, t_parse *parse)
{
	if (!handle_token_expansion(cmd, ctx, parse))
		return (0);
	else if (parse->current->type == TOK_REDIR_FROM
		|| parse->current->type == TOK_REDIR_TO
		|| parse->current->type == TOK_HERE_DOC_FROM
		|| parse->current->type == TOK_HERE_DOC_TO)
	{
		if (!parse_redirection(parse, cmd, ctx))
			return (0);
	}
	else
		advance_parse(parse);
	return (1);
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
	int			status;

	cmd = create_command();
	if (!cmd)
		return (NULL);
	while (parse->current && parse->current->type != TOK_PIPE
		&& parse->current->type != TOK_EOF)
	{
		status = process_command_token(cmd, ctx, parse);
		if (!status)
		{
			free_command(cmd);
			return (NULL);
		}
	}
	return (cmd);
}
