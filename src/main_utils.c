/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 17:47:37 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/26 12:39:56 by elagouch         ###   ########.fr       */
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
 * @brief Creates a prompt string based on previous command status
 *
 * @param prev_status Exit status of previous command
 * @return Formatted prompt string
 */
char	*create_prompt(int prev_status)
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
char	*get_user_input(t_ctx *ctx, int prev_status)
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
 * @brief Process the parsed command
 *
 * @param ctx Shell context
 * @param status Previous command exit status
 * @param input User input line
 * @return int New command exit status
 */
int	handle_command_in_main_loop(t_ctx *ctx, int status, char *input)
{
	int	new_status;

	new_status = status;
	if (parse_user_input(ctx, input))
		new_status = process_command(ctx, status);
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
	return (new_status);
}
