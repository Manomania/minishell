/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 18:10:00 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/02 16:55:53 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "execute.h"
#include "minishell.h"
#include "validation.h"

/**
 * @brief Main command loop for the shell
 *
 * Reads user input, parses it into tokens and commands,
 * then executes the commands
 *
 * @param ctx Shell context
 * @return int Final exit status
 */
static int	command_loop(t_ctx *ctx)
{
	char	*input;

	while (!ctx->exit_requested)
	{
		setup_signals();
		input = readline("$ ");
		if (!input)
			break ;
		if (*input)
			add_history(input);
		if (validate_input_length(input, ctx))
		{
			ctx->tokens = tokenize(ctx, input);
			if (ctx->tokens && validate_token_sequence(ctx->tokens))
			{
				ctx->cmd = command_parse(ctx, ctx->tokens);
				if (ctx->cmd)
					execute_commands(ctx, ctx->cmd);
			}
		}
		// Cleanup for this iteration
		if (ctx->tokens)
		{
			free_all_token(ctx->tokens);
			ctx->tokens = NULL;
		}
		if (ctx->cmd)
		{
			free_all_commands(ctx->cmd);
			ctx->cmd = NULL;
		}
		free(input);
		update_signal_status(ctx);
	}
	return (ctx->exit_status);
}

/**
 * @brief Main entrypoint for the minishell program
 *
 * Initializes context, sets up signals, runs command loop
 *
 * @param argc Argument count
 * @param argv Argument values
 * @param envp Environment variables array
 * @return int Exit status
 */
int	main(int argc, char **argv, char **envp)
{
	t_ctx	*ctx;
	int		final_status;

	ctx = init_ctx(argc, argv, envp);
	setup_signals();
	final_status = command_loop(ctx);
	ctx_clear(ctx);
	return (final_status);
}
