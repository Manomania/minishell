/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 17:19:32 by maximart          #+#    #+#             */
/*   Updated: 2025/03/08 14:31:59 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Main loop of the app
 *
 * @param ctx Context
 * @return t_bool Whether to break the loop or not
 */
static t_bool	main_loop(t_ctx *ctx)
{
	char	*input;

	input = readline("$> ");
	if (!input)
		return (true);
	if (input[0] != '\0')
		add_history(input);
	ctx->tokens = tokenize(input);
	ctx->cmd = command_parse(ctx);
	if (!ctx->cmd)
		ctx_error(ERR_UNIMPLEMENTED);
	else
	{
		command_execute(ctx, ctx->cmd);
		command_free(ctx->cmd);
		ctx->cmd = NULL;
	}
	free_all_token(ctx->tokens);
	return (false);
}

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

	ctx = ctx_init(argc, argv, envp);
	while (1)
		if (main_loop(ctx))
			break ;
	ctx_clear(ctx);
	return (EXIT_SUCCESS);
}
