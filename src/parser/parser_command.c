/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_command.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 14:37:01 by maximart          #+#    #+#             */
/*   Updated: 2025/03/17 13:55:59 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Adds an argument to a command
 *
 * This function adds a new argument to a command's argument list,
 * allocating memory as needed.
 *
 * @param cmd Command to add the argument to
 * @param value Value of the argument to add
 * @return 1 on success, 0 on failure
 */
int	add_argument(t_command *cmd, char *value)
{
	char	**new_args;
	int		i;

	if (!cmd || !value)
		return (0);
	new_args = malloc(sizeof(char *) * (size_t)(cmd->arg_count + 2));
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
	current->next = redirection;
}

/**
 * @brief Parses a redirection token
 *
 * This function parses a redirection token and adds it to the command.
 *
 * @param parse Parse context containing tokens
 * @param cmd Command to add the redirection to
 * @return 1 on success, 0 on failure
 */
int	parse_redirection(t_parse *parse, t_command *cmd)
{
	t_token_type	type;
	t_redirection	*redirection;

	type = parse->current->type;
	advance_parse(parse);
	if (!parse->current || parse->current->type != TOK_WORD)
	{
		ft_printf(RED "error: Expected filename after redirection\n" RESET);
		return (0);
	}
	redirection = create_redirection(type, parse->current->value);
	if (!redirection)
		return (0);
	add_redirection(cmd, redirection);
	advance_parse(parse);
	return (1);
}

/**
 * @brief Initializes the arguments array for a command
 *
 * This function initializes the arguments array with the first argument.
 *
 * @param cmd Command to initialize arguments for
 * @param first_arg Value of the first argument
 * @return 1 on success, 0 on failure
 */
static int	init_cmd_args(t_command *cmd, char *first_arg)
{
	cmd->args = malloc(sizeof(char *) * 2);
	if (!cmd->args)
		return (0);
	cmd->args[0] = ft_strdup(first_arg);
	if (!cmd->args[0])
	{
		free(cmd->args);
		cmd->args = NULL;
		return (0);
	}
	cmd->args[1] = NULL;
	cmd->arg_count = 0;
	return (1);
}

/**
 * @brief Processes a word token during command parsing
 *
 * This function processes a word token and adds it to the command.
 *
 * @param cmd Command to add the word to
 * @param token Current token being processed
 * @return 1 on success, 0 on failure
 */
static int	process_word_token(t_command *cmd, t_token *token)
{
	if (!cmd->args)
	{
		init_cmd_args(cmd, token->value);
		if (!cmd->args)
			return (0);
	}
	else if (add_argument(cmd, token->value) == 0)
		return (0);
	return (1);
}

/**
 * @brief Parses a command from tokens
 *
 * This function constructs a command by processing tokens until a pipe
 * or operator is encountered.
 *
 * @param parse Parse context containing tokens
 * @return Parsed command or NULL on failure
 */
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
			if (!process_word_token(cmd, parse->current))
				return (free_command(cmd), NULL);
			advance_parse(parse);
		}
		else if (token_is_redirection(parse->current->type))
		{
			if (!parse_redirection(parse, cmd))
				return (free_command(cmd), NULL);
		}
		else
			advance_parse(parse);
	}
	return (cmd);
}

/**
 * @brief Parses a token linked list into a command
 *
 * This function initializes a parsing context and parses tokens into commands.
 *
 * @param token Token linked list to parse
 * @return Resulting command structure
 */
t_command	*parse_token(t_token *token)
{
	t_parse		parse;
	t_command	*cmd;

	init_parse_context(&parse, token);
	if (parse.current->type == TOK_AND || parse.current->type == TOK_OR
		|| parse.current->type == TOK_PIPE)
	{
		ft_printf(RED "error: Unexpected token at start of command\n" RESET);
		return (NULL);
	}
	cmd = parse_command_sequence(&parse);
	if (!cmd)
		return (NULL);
	if (parse.current->type != TOK_EOF)
	{
		ft_printf(RED "error:\nUnexpected token\n" RESET);
		free_all_commands(cmd);
		return (NULL);
	}
	return (cmd);
}
