/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 19:20:06 by maximart          #+#    #+#             */
/*   Updated: 2025/03/06 14:35:56 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	cleanup(t_token *tokens)
{
	if (tokens)
		free_all_token(tokens);
	clear_history();
	rl_clear_history();
	rl_free_line_state();
	rl_cleanup_after_signal();
}

void	handle_signal(int sig)
{
	(void)sig;
	ft_printf("\nExiting...\n");
	cleanup(NULL);
	exit(0);
}

#include <signal.h>

int	main(void)
{
	char	*input;
	t_token	*tokens;

	signal(SIGINT, handle_signal);
	ft_printf(GREEN "=== Minishell Lexer/Tokenizer Tester ===\n" RESET);
	ft_printf("Type 'exit' to quit\n\n");
	while (1)
	{
		input = readline(YELLOW "minishell$ " RESET);
		if (!input)
		{
			ft_printf("\nExiting...\n");
			cleanup(NULL);
			break ;
		}
		if (input[0] != '\0')
			add_history(input);
		if (strcmp(input, "exit") == 0)
		{
			free(input);
			cleanup(NULL);
			break ;
		}
		ft_printf(YELLOW "Input: \"%s\"\n" RESET, input);
		tokens = tokenize(input);
		if (tokens)
		{
			print_tokens_list(tokens);
			free_all_token(tokens);
		}
		else
			ft_printf(RED "ERROR: Tokenization failed\n" RESET);
	}
	return (0);
}
