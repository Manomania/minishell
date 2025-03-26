/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 14:36:02 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/26 12:47:37 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "debug.h"
#include "minishell.h"

void	print_token_type(t_token_type type)
{
	if (type == TOK_WORD)
		ft_printf("WORD");
	else if (type == TOK_REDIR_FROM)
		ft_printf("REDIR_FROM");
	else if (type == TOK_REDIR_TO)
		ft_printf("REDIR_TO");
	else if (type == TOK_HERE_DOC_FROM)
		ft_printf("HERE_DOC_FROM");
	else if (type == TOK_HERE_DOC_TO)
		ft_printf("HERE_DOC_TO");
	else if (type == TOK_PIPE)
		ft_printf("PIPE");
	else if (type == TOK_NEW_LINE)
		ft_printf("NEW_LINE");
	else if (type == TOK_EOF)
		ft_printf("EOF");
	else
		ft_printf("UNKNOWN");
}

void	print_single_token(t_token *token, int count)
{
	char	*display_value;

	ft_printf(GREEN "Token %d: " RESET, count);
	ft_printf("Type: " YELLOW);
	print_token_type(token->type);
	ft_printf(RESET);
	if (token->value)
	{
		display_value = get_display_value(token->value);
		ft_printf(" | Value: " GREEN "%s" RESET, display_value);
		free(display_value);
	}
	else
		ft_printf(" | Value: " GREEN "NULL" RESET);
	ft_printf("\n");
}

void	print_tokens(t_token *tokens)
{
	t_token	*current;
	int		token_count;

	current = tokens;
	token_count = 0;
	ft_printf(YELLOW "\n===== TOKEN LIST =====\n" RESET);
	if (!current)
	{
		ft_printf(RED "No tokens to display\n" RESET);
		ft_printf(YELLOW "=====================\n\n" RESET);
		return ;
	}
	while (current)
	{
		print_single_token(current, token_count);
		token_count++;
		current = current->next;
	}
	ft_printf(YELLOW "=====================\n\n" RESET);
}

void	cleanup(t_token *tokens)
{
	if (tokens)
		free_all_token(tokens);
	clear_history();
	rl_clear_history();
	rl_free_line_state();
	rl_cleanup_after_signal();
}

void	handle_signal(int sig)
{
	(void)sig;
	ft_printf("\nExiting...\n");
	cleanup(NULL);
	exit(0);
}

// #include <signal.h>

// int	main(int argc, char **argv, char **envp)
// {
// 	char		*input;
// 	t_token		*tokens;
// 	t_command	*commands;
// 	t_ctx		*ctx;
// 	int			status;

// 	(void)argc;
// 	(void)argv;
// 	ctx = init_ctx(argc, argv, envp);
// 	if (!ctx)
// 		return (1);
// 	status = 0;
// 	while (status == 0)
// 	{
// 		input = readline(YELLOW "minishell$ " RESET);
// 		if (!input)
// 			break ;
// 		if (input[0] != '\0')
// 			add_history(input);
// 		if (ft_strncmp(input, "exit", ft_strlen(input)) == 0)
// 		{
// 			free(input);
// 			break ;
// 		}
// 		tokens = tokenize(NULL, input);
// 		if (tokens)
// 		{
// 			print_tokens(tokens);
// 			commands = parse_token(tokens, ctx);
// 			if (commands)
// 			{
// 				print_commands(commands);
// 				free_all_commands(commands);
// 			}
// 			free_all_token(tokens);
// 		}
// 		free(input);
// 	}
// 	free_env_list(ctx->env_list);
// 	free(ctx);
// 	return (0);
// }

// /**
//  * @brief Processes a single line from input file
//  *
//  * @param ctx Shell context
//  * @param line Line to process
//  * @return 0 to continue, 1 to exit
//  */
// static int process_line(t_ctx *ctx, char *line)
// {
// 	t_token		*tokens;
// 	t_command	*commands;
//
// 	if (!line || line[0] == '\0')
// 		return (0);
// 	if (line[0] == '#')
// 		return (0);
// 	if (ft_strncmp(line, "exit", ft_strlen("exit")) == 0
// 		&& (line[4] == ' ' || line[4] == '\0' || line[4] == '\n'))
// 		return (1);
// 	ft_printf(BLUE"[Command]"RESET" %s\n", line);
// 	tokens = tokenize(line);
// 	if (!tokens)
// 	{
// 		ft_printf(RED"Error: Failed to tokenize input\n"RESET);
// 		return (0);
// 	}
// 	print_tokens(tokens);
// 	commands = parse_token(tokens, ctx);
// 	if (!commands)
// 	{
// 		ft_printf(RED"Error: Failed to parse tokens\n"RESET);
// 		free_all_token(tokens);
// 		return (0);
// 	}
// 	print_commands(commands);
// 	free_all_commands(commands);
// 	free_all_token(tokens);
// 	return (0);
// }
//
// /**
//  * @brief Processes commands from a file
//  *
//  * @param ctx Shell context
//  * @param filename Name of file to read commands from
//  * @return Exit status
//  */
// static int process_file(t_ctx *ctx, char *filename)
// {
// 	int		fd;
// 	char	*line;
// 	int		exit_status;
//
// 	fd = open(filename, O_RDONLY);
// 	if (fd < 0)
// 	{
// 		ft_printf(RED"Error: Could not open file %s\n"RESET, filename);
// 		return (1);
// 	}
// 	ft_printf(GREEN"Processing commands from file: %s\n"RESET, filename);
// 	exit_status = 0;
// 	while (1)
// 	{
// 		line = get_next_line(fd);
// 		if (!line)
// 			break ;
// 		if (line[ft_strlen(line) - 1] == '\n')
// 			line[ft_strlen(line) - 1] = '\0';
//
// 		if (process_line(ctx, line))
// 		{
// 			exit_status = ctx->exit_status;
// 			free(line);
// 			break ;
// 		}
// 		free(line);
// 	}
// 	close(fd);
// 	return (exit_status);
// }
//
// int	main(int argc, char **argv, char **envp)
// {
// 	t_ctx	*ctx;
// 	int		exit_status;
//
// 	ctx = init_ctx(envp);
// 	if (!ctx)
// 		return (1);
// 	ctx->interactive = 0;
// 	if (argc < 2)
// 	{
// 		ft_printf(RED"Usage: %s <filename>\n"RESET, argv[0]);
// 		free_ctx(ctx);
// 		return (1);
// 	}
// 	exit_status = process_file(ctx, argv[1]);
// 	free_ctx(ctx);
// 	return (exit_status);
// }
