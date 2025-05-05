/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 18:10:00 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/05 15:27:58 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "execute.h"
#include "minishell.h"
#include "validation.h"

/**
 * @brief Handle user input and command parsing
 *
 * Reads user input, validates it, and parses into tokens
 *
 * @param ctx Shell context
 * @param input User input string
 * @return int 1 on success, 0 if input should be skipped
 */
static int	handle_input_parsing(t_ctx *ctx, char *input)
{
	ctx->tokens = tokenize(ctx, input);
	if (ctx->tokens && validate_token_sequence(ctx->tokens))
	{
		ctx->cmd = command_parse(ctx, ctx->tokens);
		if (ctx->cmd)
		{
			execute_commands(ctx, ctx->cmd);
			return (1);
		}
	}
	else
		ctx->exit_status = 2;
	return (1);
}

/**
 * @brief Handle shell input processing
 *
 * Reads and processes user input in the shell
 *
 * @param ctx Shell context
 * @param input User input string pointer
 * @return int 0 if user input is NULL (exit), 1 otherwise
 */
static int	process_shell_input(t_ctx *ctx, char **input)
{
	*input = readline("$ ");
	if (!*input)
		return (0);
	update_signal_status(ctx);
	if (**input)
		add_history(*input);
	if (validate_input_length(*input, ctx))
		handle_input_parsing(ctx, *input);
	return (1);
}

/**
 * @brief Main command loop for the shell
 *
 * Reads user input and manages the main shell loop
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
		if (!process_shell_input(ctx, &input))
		{
			ft_printf("exit\n");
			break ;
		}
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
