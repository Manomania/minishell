/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_utils2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 15:56:37 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/21 15:59:46 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"
#include "validation.h"

/**
 * @brief Gets user input with appropriate prompt
 * Displays prompt and reads input, handling SIGINT if received
 *
 * @param ctx Context containing shell state
 * @param prev_status Exit status of previous command
 * @return User input string or NULL on error/EOF
 */
char	*get_user_input(t_ctx *ctx, int prev_status)
{
	char	*prompt;
	char	*input;

	update_signal_status(ctx);
	prompt = create_prompt(prev_status);
	if (!prompt)
		ctx_error_exit(ctx, NULL, "prompt", ERR_ALLOC);
	input = readline(prompt);
	free(prompt);
	update_signal_status(ctx);
	if (!input)
	{
		ft_printf("exit\n");
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
 * @brief Process the parsed command
 * Handles command execution and resource cleanup
 *
 * @param ctx Context containing shell state
 * @param input User input line
 */
void	handle_command_in_main_loop(t_ctx *ctx, char *input)
{
	if (parse_user_input(ctx, input))
		process_command(ctx);
	else
	{
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
	}
}
