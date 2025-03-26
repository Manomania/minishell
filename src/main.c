/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 18:10:00 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/26 12:40:23 by elagouch         ###   ########.fr       */
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
 */
static void	command_loop(t_ctx *ctx, int prev_status)
{
	char	*input;
	int		status;
	int		running;

	status = prev_status;
	running = 1;
	while (running)
	{
		input = get_user_input(ctx, status);
		if (!input)
			ctx_exit(ctx);
		debug_log(DEBUG_INFO, "main", "Processing user input");
		status = handle_command_in_main_loop(ctx, status, input);
		if (ctx->exit_requested)
			running = 0;
	}
}

/**
 * @brief Checks command line args for debug flags
 *
 * @param argc Argument count
 * @param argv Argument values
 */
static void	check_debug_args(int argc, char **argv)
{
	int	i;

	i = 1;
	while (i < argc)
	{
		if (ft_strncmp(argv[i], "--debug", ft_strlen("--debug")) == 0)
			debug_init(DEBUG_INFO);
		else if (ft_strncmp(argv[i], "--verbose", ft_strlen("--verbose")) == 0)
			debug_init(DEBUG_VERBOSE);
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

	check_debug_args(argc, argv);
	debug_log(DEBUG_INFO, "main", "Initializing minishell");
	ctx = init_ctx(argc, argv, envp);
	if (!ctx)
		error_exit(NULL, ERR_ALLOC, "context initialization");
	setup_signals();
	debug_log(DEBUG_INFO, "main", "Starting command loop");
	command_loop(ctx, 0);
	ctx_clear(ctx);
	return (EXIT_SUCCESS);
}
