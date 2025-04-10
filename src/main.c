/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 18:10:00 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/07 19:30:53 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "debug.h"
#include "error.h"
#include "minishell.h"
#include "validation.h"

/**
 * @brief Main command loop for the shell
 *
 * @param ctx Shell context
 * @param prev_status Previous command exit status
 * @return int Final exit status
 */
static int	command_loop(t_ctx *ctx)
{
	char	*input;
	int		running;

	ctx->exit_status = 0;
	running = 1;
	while (running)
	{
		input = get_user_input(ctx, ctx->exit_status);
		if (!input)
			return (ctx->exit_status);
		handle_command_in_main_loop(ctx, input);
		if (ctx->exit_requested)
			running = 0;
	}
	return (ctx->exit_status);
}

/**
 * @brief Checks command line args for debug flags
 *
 * @param ctx Context
 * @param argc Argument count
 * @param argv Argument values
 */
static void	check_debug_args(t_ctx *ctx, int argc, char **argv)
{
	int	i;

	i = 1;
	while (i < argc)
	{
		if (ft_strncmp(argv[i], "--debug", ft_strlen("--debug")) == 0)
			ctx->debug = true;
		i++;
	}
}

/**
 * @brief Main entrypoint for the minishell program
 *
 * @param argc Argument count
 * @param argv Argument values
 * @param envp Environment variables array
 * @return int Exit status
 */
int	main(int argc, char **argv, char **envp)
{
	t_ctx	*ctx;
	int	final_status;

	ctx = init_ctx(argc, argv, envp);
	check_debug_args(ctx, argc, argv);
	if (!ctx)
		ctx_error_exit(ctx, NULL, "main", ERR_ALLOC);
	setup_signals();
	final_status = command_loop(ctx);
	final_cleanup(ctx);
	return (final_status);
}
