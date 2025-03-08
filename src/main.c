/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 17:19:32 by maximart          #+#    #+#             */
/*   Updated: 2025/03/08 12:23:30 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Main entrypoint
 *
 * @param argc Arguments count
 * @param argv Arguments
 * @param envp Environment variables
 * @return int Exit code
 */
int	main(int argc, char **argv, char **envp)
{
	t_ctx	*ctx;
	char	*input;

	(void)argc;
	(void)argv;
	(void)envp;
	ctx = ctx_init();
	while (1)
	{
		input = readline("$> ");
		if (!input)
			break ;
		if (input[0] != '\0')
			add_history(input);
		ctx->tokens = tokenize(input);
		cmds_handle(ctx);
		free_all_token(ctx->tokens);
	}
	ctx_clear(ctx);
	return (EXIT_SUCCESS);
}
