/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 18:10:00 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/02 15:34:01 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"
#include "validation.h"

/**
 * @brief Main command loop for the shell
 *
 * @param ctx Shell context
 * @return int Final exit status
 */
static int	command_loop(t_ctx *ctx)
{
	// ...
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
	int		final_status;

	ctx = init_ctx(argc, argv, envp);
	// setup_signals(); // Will have to be used eventually to handle signals
	final_status = command_loop(ctx);
	ctx_clear(ctx); // Final cleanup
	return (final_status);
}
