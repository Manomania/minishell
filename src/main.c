/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 18:10:00 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/17 15:41:46 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "debug.h"
#include "error.h"
#include "minishell.h"
#include "validation.h"

/**
 * @brief Creates a prompt string based on previous command status
 *
 * @param prev_status Exit status of previous command
 * @return Formatted prompt string
 */
static char	*create_prompt(int prev_status)
{
	char	*rdl_str1;
	char	*rdl_str2;
	char	*rdl_str3;
	char	*prompt;

	prompt = NULL;
	if (prev_status > 0)
	{
		rdl_str1 = ft_itoa(prev_status);
		if (!rdl_str1)
			return (NULL);
		rdl_str2 = ft_strjoin("\001" RED "\002", rdl_str1);
		free(rdl_str1);
		if (!rdl_str2)
			return (NULL);
		rdl_str3 = ft_strjoin(rdl_str2, " $ \001" RESET "\002");
		free(rdl_str2);
		prompt = rdl_str3;
	}
	else
		prompt = ft_strdup("\001" GREEN "\002$ \001" RESET "\002");
	return (prompt);
}

/**
 * @brief Gets user input with appropriate prompt
 *
 * @param ctx Shell context
 * @param prev_status Exit status of previous command
 * @return User input string or NULL on error/EOF
 */
static char	*get_user_input(t_ctx *ctx, int prev_status)
{
	char	*prompt;
	char	*input;

	prompt = create_prompt(prev_status);
	if (!prompt)
		error_exit(ctx, ERR_ALLOC, "prompt");
	input = readline(prompt);
	free(prompt);
	if (!input)
	{
		ft_putstr("exit\n");
		return (NULL);
	}
	if (input[0] != '\0')
		add_history(input);
	if (!validate_input_length(input, ctx))
	{
		free(input);
		return (NULL);
	}
	return (input);
}

/**
 * @brief Processes a command after parsing
 *
 * @param ctx Shell context
 * @param prev_status Previous command exit status
 * @return Exit status of the command
 */
static int	process_command(t_ctx *ctx, int prev_status)
{
	int		status;
	t_bool	should_exit;

	should_exit = false;
	status = prev_status;
	if (ctx->cmd && ctx->cmd->args && ctx->cmd->args[0]
		&& ft_strncmp(ctx->cmd->args[0], "exit", __INT_MAX__) == 0)
	{
		ft_putstr("exit\n");
		should_exit = true;
	}
	if (!should_exit)
		status = command_execute(ctx);
	if (ctx->cmd)
		free_all_commands(ctx->cmd);
	ctx->cmd = NULL;
	if (ctx->tokens)
		free_all_token(ctx->tokens);
	ctx->tokens = NULL;
	if (status == -1)
		status = prev_status;
	if (should_exit)
		ctx->exit_status = 1;
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
	ctx->tokens = tokenize(input);
	free(input);
	if (!ctx->tokens)
		return (false);
	debug_print_tokens(DEBUG_VERBOSE, ctx->tokens);
	ctx->cmd = command_parse(ctx->tokens);
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
		if (ctx->exit_status)
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
	ctx = ctx_init(argc, argv, envp);
	if (!ctx)
		error_exit(NULL, ERR_ALLOC, "context initialization");
	setup_signals();
	debug_log(DEBUG_INFO, "main", "Starting command loop");
	command_loop(ctx, 0);
	ctx_clear(ctx);
	return (EXIT_SUCCESS);
}
