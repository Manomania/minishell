/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 17:19:32 by maximart          #+#    #+#             */
/*   Updated: 2025/03/08 18:11:34 by elagouch         ###   ########.fr       */
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
	char		*input;
	t_pipeline	*pipeline;
	t_command	*cmd;
	int			status;

	input = readline("$> ");
	if (!input)
		return (true);
	if (input[0] != '\0')
		add_history(input);
	ctx->tokens = tokenize(input);
	free(input);
	if (!ctx->tokens)
		return (false);
	pipeline = pipeline_parse(ctx);
	if (!pipeline)
	{
		free_all_token(ctx->tokens);
		ctx->tokens = NULL;
		return (false);
	}
	cmd = pipeline->commands;
	if (cmd && cmd->cmd && ft_strncmp(cmd->cmd, "exit", __INT_MAX__) == 0)
	{
		pipeline_free(pipeline);
		free_all_token(ctx->tokens);
		ctx->tokens = NULL;
		return (true);
	}
	status = pipeline_execute(ctx, pipeline);
	pipeline_free(pipeline);
	free_all_token(ctx->tokens);
	ctx->tokens = NULL;
	ft_printf("Return code: '%d'\n", status);
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
	setup_signals();
	while (1)
		if (main_loop(ctx))
			break ;
	ctx_clear(ctx);
	return (EXIT_SUCCESS);
}
