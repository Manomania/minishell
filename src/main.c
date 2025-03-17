/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 17:19:32 by maximart          #+#    #+#             */
/*   Updated: 2025/03/17 15:25:11 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// static void	display_args(t_ctx *ctx)
// {
// 	char	**args;
// 	int		i;

// 	if (!ctx->cmd || !ctx->cmd->args)
// 	{
// 		ft_printf("No command found!\n");
// 		return ;
// 	}
// 	ft_printf("Command: %s\n", ctx->cmd->cmd);
// 	if (ctx->cmd->arg_count == 0)
// 	{
// 		ft_printf("No arguments provided\n");
// 		return ;
// 	}
// 	ft_printf("Arguments (%d):\n", ctx->cmd->arg_count);
// 	args = ctx->cmd->args;
// 	i = 1;
// 	while (args[i])
// 	{
// 		ft_printf("  Arg[%d]: '%s'\n", i - 1, args[i]);
// 		i++;
// 	}
// }

static char	*prompted_input(int prev_status)
{
	char	*rdl_str1;
	char	*rdl_str2;
	char	*rdl_str3;
	char	*input;

	if (prev_status > 0)
	{
		rdl_str1 = ft_itoa(prev_status);
		rdl_str2 = ft_strjoin("\001" RED "\002", rdl_str1);
		rdl_str3 = ft_strjoin(rdl_str2, " $ \001" RESET "\002");
		if (!rdl_str3)
			prev_status = -1;
		input = readline(rdl_str3);
		free(rdl_str1);
		free(rdl_str2);
		free(rdl_str3);
	}
	if (prev_status <= 0)
		input = readline("\001" GREEN "\002$ \001" RESET "\002");
	return (input);
}

static void	main_loop_end(t_ctx *ctx, int prev_status);

static void	*main_loop(t_ctx *ctx, int prev_status)
{
	char	*input;

	input = prompted_input(prev_status);
	if (!input)
	{
		ft_putstr("exit");
		ctx_exit(ctx);
	}
	if (input[0] != '\0')
		add_history(input);
	ctx->tokens = tokenize(input);
	free(input);
	if (!ctx->tokens)
		return (main_loop(ctx, 0));
	ctx->cmd = command_parse(ctx->tokens);
	if (!ctx->cmd)
	{
		free_all_token(ctx->tokens);
		ctx->tokens = NULL;
		return (main_loop(ctx, 0));
	}
	main_loop_end(ctx, prev_status);
	return (NULL);
}

/**
 * @brief Processes input and executes commands
 *
 * @param ctx Context containing environment and state
 * @return t_bool true if the loop should exit, false otherwise
 */
static void	main_loop_end(t_ctx *ctx, int prev_status)
{
	t_bool	should_exit;
	int		status;

	should_exit = false;
	if (ctx->cmd->args && ctx->cmd->args[0] && ft_strncmp(ctx->cmd->args[0],
			"exit", __INT_MAX__) == 0)
	{
		ft_putstr("exit");
		status = 0;
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
	if (!should_exit)
		(void)main_loop(ctx, status);
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
	main_loop(ctx, 0);
	ctx_clear(ctx);
	return (EXIT_SUCCESS);
}
