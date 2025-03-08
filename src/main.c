/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 17:19:32 by maximart          #+#    #+#             */
/*   Updated: 2025/03/08 11:16:00 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	main(void)
{
	t_ctx	*ctx;
	char	*input;

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
		// print_tokens_list(ctx->tokens);
		free_all_token(ctx->tokens);
	}
	ctx_clear(ctx);
	return (EXIT_SUCCESS);
}
