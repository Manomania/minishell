/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 18:10:00 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/19 18:35:19 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "debug.h"
#include "error.h"
#include "minishell.h"
#include "validation.h"

/**
 * @brief Processes a command after parsing
 *
 * @param ctx Shell context
 * @param prev_status Previous command exit status
 * @return Exit status of the command
 */
static int	process_command(t_ctx *ctx, int prev_status)
{
	int	status;

	status = prev_status;
	if (ctx->cmd && ctx->cmd->args && ctx->cmd->args[0]
		&& ft_strncmp(ctx->cmd->args[0], "exit", __INT_MAX__) == 0)
		ft_putstr("exit\n");
	status = command_execute(ctx);
	if (ctx->cmd)
		free_all_commands(ctx->cmd);
	ctx->cmd = NULL;
	if (ctx->tokens)
		free_all_token(ctx->tokens);
	ctx->tokens = NULL;
	if (status == -1)
		status = prev_status;
	if (!ctx->exit_requested)
		ctx->exit_status = status;
	return (status);
}

/**
 * @brief Tokenizes and parses user input
 *
 * @param ctx Shell context
 * @param input User input string
 * @return true if successful, false on error
 */
static t_bool	parse_user_input(t_ctx *ctx, char *input)
{
	ctx->tokens = tokenize(ctx, input);
	if (!validate_token_sequence(ctx->tokens))
	{
		// Handle syntax error
		free_all_token(ctx->tokens);
		ctx->tokens = NULL;
		return (false);
	}
	if (g_debug_level > INFO)
		print_tokens(ctx->tokens);
	free(input);
	if (!ctx->tokens)
		return (false);
	debug_print_tokens(DEBUG_VERBOSE, ctx->tokens);
	ctx->cmd = command_parse(ctx, ctx->tokens);
	if (!ctx->cmd)
	{
		free_all_token(ctx->tokens);
		ctx->tokens = NULL;
		return (false);
	}
	debug_print_commands(DEBUG_VERBOSE, ctx->cmd);
	if (!validate_command(ctx->cmd, ctx))
	{
		free_all_token(ctx->tokens);
		ctx->tokens = NULL;
		free_all_commands(ctx->cmd);
		ctx->cmd = NULL;
		return (false);
	}
	return (true);
}

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
		if (parse_user_input(ctx, input))
			status = process_command(ctx, status);
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
