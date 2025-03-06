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


void print_tokens_list(t_token *tokens)
{
	t_token *current = tokens;
	int token_count = 0;

	ft_printf(YELLOW"\n===== TOKEN LIST =====\n"RESET);
	while (current)
	{
		ft_printf(GREEN"Token %d: "RESET, token_count++);
		ft_printf("Type: "YELLOW);
		switch (current->type)
		{
			case TOK_WORD: ft_printf("WORD"); break;
			case TOK_REDIR_FROM: ft_printf("REDIR_FROM"); break;
			case TOK_REDIR_TO: ft_printf("REDIR_TO"); break;
			case TOK_HERE_DOC_FROM: ft_printf("HERE_DOC_FROM"); break;
			case TOK_HERE_DOC_TO: ft_printf("HERE_DOC_TO"); break;
			case TOK_PIPE: ft_printf("PIPE"); break;
			case TOK_ENV: ft_printf("ENV"); break;
			case TOK_AND: ft_printf("AND"); break;
			case TOK_OR: ft_printf("OR"); break;
			case TOK_NEW_LINE: ft_printf("NEW_LINE"); break;
			case TOK_EOF: ft_printf("EOF"); break;
			default: ft_printf("UNKNOWN"); break;
		}
		ft_printf(RESET);
		if (current->value)
		ft_printf(" | Value: "GREEN"%s"RESET, current->value);
		else
		ft_printf(" | Value: "GREEN"NULL"RESET);
		ft_printf("\n");
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

int main(void)
{
	char *input;
	t_token *tokens;
	signal(SIGINT, handle_signal);
	ft_printf(GREEN"=== Minishell Lexer/Tokenizer Tester ===\n"RESET);
	ft_printf("Type 'exit' to quit\n\n");
	while (1)
	{
		input = readline(YELLOW"minishell$ "RESET);
	if (!input)
	{
		ft_printf("\nExiting...\n");
		cleanup(NULL);
		break;
	}
	if (input[0] != '\0')
		add_history(input);
	if (strcmp(input, "exit") == 0)
	{
		free(input);
		cleanup(NULL);
		break;
	}
	ft_printf(YELLOW"Input: \"%s\"\n"RESET, input);
	tokens = tokenize(input);
	if (tokens)
	{
		print_tokens_list(tokens);
		free_all_token(tokens);
	}
	else
		ft_printf(RED"ERROR: Tokenization failed\n"RESET);
	}
	return 0;
}