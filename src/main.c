/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 17:19:32 by maximart          #+#    #+#             */
/*   Updated: 2025/03/11 12:11:49 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Processes input and executes commands
 *
 * @param ctx Context containing environment and state
 * @return t_bool true if the loop should exit, false otherwise
 */
static t_bool	main_loop(t_ctx *ctx)
{
	t_bool	should_exit;
	char	*input;
	int		status;

	should_exit = false;
	input = readline("$ ");
	if (!input)
		return (true);
	if (input[0] != '\0')
		add_history(input);
	ctx->tokens = tokenize(input);
	free(input);
	if (!ctx->tokens)
		return (false);
	ctx->cmd = command_parse(ctx->tokens);
	if (!ctx->cmd)
	{
		ctx_error(ERR_UNIMPLEMENTED);
		free_all_token(ctx->tokens);
		ctx->tokens = NULL;
		return (true);
	}
	if (ctx->cmd->cmd && ft_strncmp(ctx->cmd->cmd, "exit", __INT_MAX__) == 0)
		should_exit = true;
	if (!should_exit)
	{
		status = command_execute(ctx);
		ft_printf("Return code: '%d'\n", status);
	}
	command_free(ctx->cmd);
	ctx->cmd = NULL;
	free_all_token(ctx->tokens);
	ctx->tokens = NULL;
	return (should_exit);
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
	setup_signals();
	while (1)
		if (main_loop(ctx))
			break ;
	ctx_clear(ctx);
	return (EXIT_SUCCESS);
}
