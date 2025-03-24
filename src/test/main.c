/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 17:19:32 by maximart          #+#    #+#             */
/*   Updated: 2025/03/18 12:46:40 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Gets a user input with appropriate prompt based on previous status
 *
 * @param prev_status The exit status of the previous command
 * @return char* The input string obtained from readline
 */
static char	*prompted_input(int prev_status) {
  char *rdl_str1;
  char *rdl_str2;
  char *rdl_str3;
  char *input;

  if (prev_status > 0) {
    rdl_str1 = ft_itoa(prev_status);
    rdl_str2 = ft_strjoin("\001\033[33m\002", rdl_str1);
    rdl_str3 = ft_strjoin(rdl_str2, " $ \001\033[0m\002");
    if (!rdl_str3)
      prev_status = -1;
    input = readline(rdl_str3);
    free(rdl_str1);
    free(rdl_str2);
    free(rdl_str3);
  } else
    input = readline("\001\033[32m\002$ \001\033[0m\002");
  return (input);
}

/**
 * @brief Completes the main loop process by executing commands
 *
 * @param ctx Context containing environment and state
 * @param prev_status The previous command's exit status
 */
static void		main_loop_end(t_ctx *ctx, int prev_status);

/**
 * @brief Main loop that handles input and command processing
 *
 * @param ctx Context containing environment and state
 * @param prev_status The previous command's exit status
 * @return void* Always returns NULL
 */
static void	*main_loop(t_ctx *ctx, int prev_status) {
  char *input;

  input = prompted_input(prev_status);
  if (!input)
    ctx_exit(ctx);
  if (input[0] != '\0')
    add_history(input);
  ctx->tokens = tokenize(ctx, input);
  free(input);
  if (!ctx->tokens)
    return (main_loop(ctx, 0));
  ctx->cmd = command_parse(ctx->tokens);
  if (!ctx->cmd) {
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
 * @param prev_status The previous command's exit status
 */
static void	main_loop_end(t_ctx *ctx, int prev_status) {
  t_bool should_exit;
  int status;

  should_exit = false;
  if (ctx->cmd->args && ctx->cmd->args[0] &&
      ft_strncmp(ctx->cmd->args[0], "exit", __INT_MAX__) == 0) {
    ft_putstr("exit\n");
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
  else
    ctx_clear(ctx);
}

/**
 * @brief Check if we're running in test mode (non-interactive)
 *
 * @return t_bool true if stdin is not a terminal (piped input)
 */
static t_bool	is_test_mode(void) { return (!isatty(STDIN_FILENO)); }

/**
 * @brief Main entrypoint
 *
 * @param argc Arguments count
 * @param argv Arguments
 * @param envp Environment variables
 * @return int Exit code
 */
int	main(int argc, char **argv, char **envp) {
  t_ctx *ctx;

  ctx = init_ctx(argc, argv, envp);
  setup_signals();
  if (is_test_mode())
    rl_outstream = fopen("/dev/null", "w");
  main_loop(ctx, 0);
  if (is_test_mode() && rl_outstream != NULL && rl_outstream != stdout) {
    fclose(rl_outstream);
  }
  ctx_clear(ctx);
  return (EXIT_SUCCESS);
}
