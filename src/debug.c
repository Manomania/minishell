/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maximart <maximart@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 19:20:06 by maximart          #+#    #+#             */
/*   Updated: 2025/03/05 19:20:09 by maximart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


/**
 * @brief Gets the value of an environment variable
 *
 * @param env_list Environment variable list
 * @param key Key to look for
 * @return Value of the variable or NULL if not found
 */
char	*get_env_value(t_env *env_list, char *key)
{
	t_env	*current;

	current = env_list;
	while (current)
	{
		if (ft_strncmp(current->key, key, ft_strlen(key)) == 0)
			return (current->value);
		current = current->next;
	}
	return (NULL);
}

void	print_redirection_type(t_token_type type)
{
	if (type == TOK_REDIR_FROM)
		ft_printf("< (REDIR_FROM)");
	else if (type == TOK_REDIR_TO)
		ft_printf("> (REDIR_TO)");
	else if (type == TOK_HERE_DOC_FROM)
		ft_printf("<< (HERE_DOC_FROM)");
	else if (type == TOK_HERE_DOC_TO)
		ft_printf(">> (HERE_DOC_TO)");
	else
		ft_printf("UNKNOWN");
}

void	print_redirections(t_redirection *redir)
{
	t_redirection	*current;
	int				redir_count;

	redir_count = 0;
	current = redir;
	if (!current)
	{
		ft_printf(YELLOW"  No redirections\n"RESET);
		return ;
	}
	ft_printf(YELLOW"  Redirections:\n"RESET);
	while (current)
	{
		ft_printf(GREEN"    Redirection %d: "RESET, redir_count++);
		ft_printf("Type: "YELLOW);
		print_redirection_type(current->type);
		ft_printf(RESET);
		ft_printf(" | Filename: "GREEN"%s"RESET, current->filename);
		ft_printf("\n");
		current = current->next;
	}
}

void	print_command_args(t_command *cmd)
{
	int	i;

	i = 0;
	if (!cmd->args || cmd->arg_count == 0)
	{
		ft_printf(YELLOW"  No arguments\n"RESET);
		return ;
	}
	ft_printf(YELLOW"  Arguments:\n"RESET);
	while (i < cmd->arg_count)
	{
		ft_printf(GREEN"    Arg %d: "RESET"%s\n", i, cmd->args[i]);
		i++;
	}
}

void	print_operator_type(t_token_type op_type)
{
	if (op_type == TOK_PIPE)
		ft_printf(BLUE"  Piped to next command (|)\n"RESET);
	else if (op_type == TOK_OR)
		ft_printf(BLUE"  OR operator to next command (||)\n"RESET);
	else if (op_type == TOK_AND)
		ft_printf(BLUE"  AND operator to next command (&&)\n"RESET);
}

void	print_commands(t_command *cmd)
{
	t_command	*current;
	int			cmd_count;

	cmd_count = 0;
	current = cmd;
	ft_printf(YELLOW"\n===== COMMANDS =====\n"RESET);
	if (!current)
	{
		ft_printf(RED"No commands to display\n"RESET);
		ft_printf(YELLOW"===================\n\n"RESET);
		return ;
	}
	while (current)
	{
		ft_printf(GREEN"Command %d:\n"RESET, cmd_count++);
		print_command_args(current);
		print_redirections(current->redirection);
		if (current->next)
			print_operator_type(current->operator);
		current = current->next;
	}
	ft_printf(YELLOW"===================\n\n"RESET);
}

void	print_token_type(t_token_type type)
{
	if (type == TOK_WORD)
		ft_printf("WORD");
	else if (type == TOK_REDIR_FROM)
		ft_printf("REDIR_FROM");
	else if (type == TOK_REDIR_TO)
		ft_printf("REDIR_TO");
	else if (type == TOK_HERE_DOC_FROM)
		ft_printf("HERE_DOC_FROM");
	else if (type == TOK_HERE_DOC_TO)
		ft_printf("HERE_DOC_TO");
	else if (type == TOK_PIPE)
		ft_printf("PIPE");
	else if (type == TOK_ENV)
		ft_printf("ENV");
	else if (type == TOK_AND)
		ft_printf("AND");
	else if (type == TOK_OR)
		ft_printf("OR");
	else if (type == TOK_NEW_LINE)
		ft_printf("NEW_LINE");
	else if (type == TOK_EOF)
		ft_printf("EOF");
	else
		ft_printf("UNKNOWN");
}

void	print_single_token(t_token *token, int count)
{
	ft_printf(GREEN"Token %d: "RESET, count);
	ft_printf("Type: "YELLOW);
	print_token_type(token->type);
	ft_printf(RESET);
	if (token->value)
		ft_printf(" | Value: "GREEN"%s"RESET, token->value);
	else
		ft_printf(" | Value: "GREEN"NULL"RESET);
	ft_printf("\n");
}

void	print_tokens(t_token *tokens)
{
	t_token	*current;
	int		token_count;

	current = tokens;
	token_count = 0;
	ft_printf(YELLOW"\n===== TOKEN LIST =====\n"RESET);
	if (!current)
	{
		ft_printf(RED"No tokens to display\n"RESET);
		ft_printf(YELLOW"=====================\n\n"RESET);
		return ;
	}
	while (current)
	{
		print_single_token(current, token_count);
		token_count++;
		current = current->next;
	}
	ft_printf(YELLOW"=====================\n\n"RESET);
}

void cleanup(t_token *tokens)
{
	if (tokens)
	free_all_token(tokens);
	clear_history();
	rl_clear_history();
	rl_free_line_state();
	rl_cleanup_after_signal();
}

void handle_signal(int sig)
{
	(void)sig;
	ft_printf("\nExiting...\n");
	cleanup(NULL);
	exit(0);
}

#include <signal.h>

int	main(int argc, char **argv, char **envp)
{
	char		*input;
	t_token		*tokens;
	t_command	*commands;
	t_ctx		*ctx;
	int			status;

	(void)argc;
	(void)argv;
	ctx = init_ctx(envp);
	if (!ctx)
		return (1);
	status = 0;
	while (status == 0)
	{
		input = readline(YELLOW"minishell$ "RESET);
		if (!input)
			break;
		if (input[0] != '\0')
			add_history(input);
		if (ft_strncmp(input, "exit", ft_strlen(input)) == 0)
		{
			free(input);
			break;
		}
		tokens = tokenize(input);
		if (tokens)
		{
			print_tokens(tokens);
			commands = parse_token(tokens, ctx);
			if (commands)
			{
				print_commands(commands);
				free_all_commands(commands);
			}
			free_all_token(tokens);
		}
		free(input);
	}
	free_env_list(ctx->env_list);
	free(ctx);
	return (0);
}