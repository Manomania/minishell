/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 17:47:37 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/21 15:57:58 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "error.h"
#include "minishell.h"
#include "validation.h"

/**
 * @brief Processes a command after parsing
 * Executes the command and frees related resources
 *
 * @param ctx Context containing shell state
 */
void	process_command(t_ctx *ctx)
{
	if (ctx->cmd && ctx->cmd->args && ctx->cmd->args[0])
	{
		if (ft_strncmp(ctx->cmd->args[0], "exit", __INT_MAX__) == 0)
			ft_printf("exit\n");
	}
	ctx->exit_status = command_execute(ctx);
	if (ctx->cmd)
	{
		free_all_commands(ctx->cmd);
		ctx->cmd = NULL;
	}
	if (ctx->tokens)
	{
		free_all_token(ctx->tokens);
		ctx->tokens = NULL;
	}
}

/**
 * @brief Validates and frees tokens if invalid
 *
 * @param ctx Context containing shell state
 * @param input User input string to free
 * @return t_bool false on validation error, true otherwise
 */
static t_bool	validate_tokens(t_ctx *ctx, char *input)
{
	if (!validate_token_sequence(ctx->tokens))
	{
		free_all_token(ctx->tokens);
		ctx->tokens = NULL;
		ctx->exit_status = 2;
		free(input);
		return (false);
	}
	return (true);
}

/**
 * @brief Validates and processes command structures
 *
 * @param ctx Context containing shell state
 * @return t_bool false on command error, true otherwise
 */
static t_bool	process_commands(t_ctx *ctx)
{
	t_bool	result;

	result = true;
	ctx->cmd = command_parse(ctx, ctx->tokens);
	if (!ctx->cmd)
	{
		free_all_token(ctx->tokens);
		ctx->tokens = NULL;
		return (false);
	}
	if (!validate_command(ctx->cmd, ctx))
	{
		free_all_token(ctx->tokens);
		ctx->tokens = NULL;
		free_all_commands(ctx->cmd);
		ctx->cmd = NULL;
		result = false;
	}
	return (result);
}

/**
 * @brief Tokenizes and parses user input
 * Processes input string into tokens and command structures
 *
 * @param ctx Context containing shell state
 * @param input User input string
 * @return t_bool true if successful, false on error
 */
t_bool	parse_user_input(t_ctx *ctx, char *input)
{
	t_bool	result;

	ctx->tokens = tokenize(ctx, input);
	if (!ctx->tokens)
	{
		free(input);
		return (false);
	}
	if (!validate_tokens(ctx, input))
		return (false);
	free(input);
	result = process_commands(ctx);
	return (result);
}

/**
 * @brief Creates a prompt string based on previous command status
 * Creates a colored prompt showing the exit status if non-zero
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
		rdl_str3 = ft_strjoin(rdl_str2, " $\001" RESET "\002 ");
		free(rdl_str2);
		prompt = rdl_str3;
	}
	else
		prompt = ft_strdup("\001" GREEN "\002$\001" RESET "\002 ");
	return (prompt);
}
