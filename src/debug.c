/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maximart <maximart@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 19:20:06 by maximart          #+#    #+#             */
/*   Updated: 2025/03/05 19:20:09 by maximart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


/**
 * @brief Gets the value of an environment variable
 *
 * @param env_list Environment variable list
 * @param key Key to look for
 * @return Value of the variable or NULL if not found
 */
char	*get_env_value(t_env *env_list, char *key)
{
	t_env	*current;

	current = env_list;
	while (current)
	{
		if (ft_strncmp(current->key, key, ft_strlen(key)) == 0
			&& ft_strlen(current->key) == ft_strlen(key))
			return (current->value);
		current = current->next;
	}
	return (NULL);
}

void	print_redirection_type(t_token_type type)
{
	if (type == TOK_REDIR_FROM)
		ft_printf("< (REDIR_FROM)");
	else if (type == TOK_REDIR_TO)
		ft_printf("> (REDIR_TO)");
	else if (type == TOK_HERE_DOC_FROM)
		ft_printf("<< (HERE_DOC_FROM)");
	else if (type == TOK_HERE_DOC_TO)
		ft_printf(">> (HERE_DOC_TO)");
	else
		ft_printf("UNKNOWN");
}

void	print_redirections(t_redirection *redir)
{
	t_redirection	*current;
	int				redir_count;

	redir_count = 0;
	current = redir;
	if (!current)
	{
		ft_printf(YELLOW"  No redirections\n"RESET);
		return ;
	}
	ft_printf(YELLOW"  Redirections:\n"RESET);
	while (current)
	{
		ft_printf(GREEN"    Redirection %d: "RESET, redir_count++);
		ft_printf("Type: "YELLOW);
		print_redirection_type(current->type);
		ft_printf(RESET);
		ft_printf(" | Filename: "GREEN"%s"RESET, current->filename);
		ft_printf("\n");
		current = current->next;
	}
}

static char	*get_display_value(char *str)
{
	int		i;
	int		j;
	char	*result;
	int		in_squote;
	int		in_dquote;

	if (!str)
		return (ft_strdup("NULL"));
	result = malloc(ft_strlen(str) + 1);
	if (!result)
		return (ft_strdup("[malloc error]"));
	i = 0;
	j = 0;
	in_squote = 0;
	in_dquote = 0;
	while (str[i])
	{
		if (str[i] == '\'' && !in_dquote)
			in_squote = !in_squote;
		else if (str[i] == '"' && !in_squote)
			in_dquote = !in_dquote;
		else
			result[j++] = str[i];
		i++;
	}
	result[j] = '\0';
	return (result);
}

void	print_command_args(t_command *cmd)
{
	int		i;
	char	*display_value;

	i = 0;
	if (!cmd->args || cmd->arg_count == 0)
	{
		ft_printf(YELLOW"  No arguments\n"RESET);
		return ;
	}
	ft_printf(YELLOW"  Arguments:\n"RESET);
	while (i < cmd->arg_count)
	{
		display_value = get_display_value(cmd->args[i]);
		ft_printf(GREEN"    Arg %d: "RESET"%s\n", i, display_value);
		free(display_value);
		i++;
	}
}

void	print_operator_type(t_token_type op_type)
{
	if (op_type == TOK_PIPE)
		ft_printf(BLUE"  Piped to next command (|)\n"RESET);
	else if (op_type == TOK_OR)
		ft_printf(BLUE"  OR operator to next command (||)\n"RESET);
	else if (op_type == TOK_AND)
		ft_printf(BLUE"  AND operator to next command (&&)\n"RESET);
}

void	print_commands(t_command *cmd)
{
	t_command	*current;
	int			cmd_count;

	cmd_count = 0;
	current = cmd;
	ft_printf(YELLOW"\n===== COMMANDS =====\n"RESET);
	if (!current)
	{
		ft_printf(RED"No commands to display\n"RESET);
		ft_printf(YELLOW"===================\n\n"RESET);
		return ;
	}
	while (current)
	{
		ft_printf(GREEN"Command %d:\n"RESET, cmd_count++);
		print_command_args(current);
		print_redirections(current->redirection);
		if (current->next)
			print_operator_type(current->operator);
		current = current->next;
	}
	ft_printf(YELLOW"===================\n\n"RESET);
}

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
	else if (type == TOK_ENV)
		ft_printf("ENV");
	else if (type == TOK_AND)
		ft_printf("AND");
	else if (type == TOK_OR)
		ft_printf("OR");
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

	ft_printf(GREEN"Token %d: "RESET, count);
	ft_printf("Type: "YELLOW);
	print_token_type(token->type);
	ft_printf(RESET);
	if (token->value)
	{
		display_value = get_display_value(token->value);
		ft_printf(" | Value: "GREEN"%s"RESET, display_value);
		free(display_value);
	}
	else
		ft_printf(" | Value: "GREEN"NULL"RESET);
	ft_printf("\n");
}

void	print_tokens(t_token *tokens)
{
	t_token	*current;
	int		token_count;

	current = tokens;
	token_count = 0;
	ft_printf(YELLOW"\n===== TOKEN LIST =====\n"RESET);
	if (!current)
	{
		ft_printf(RED"No tokens to display\n"RESET);
		ft_printf(YELLOW"=====================\n\n"RESET);
		return ;
	}
	while (current)
	{
		print_single_token(current, token_count);
		token_count++;
		current = current->next;
	}
	ft_printf(YELLOW"=====================\n\n"RESET);
}

void cleanup(t_token *tokens)
{
	if (tokens)
	free_all_token(tokens);
	clear_history();
	rl_clear_history();
	rl_free_line_state();
	rl_cleanup_after_signal();
}

void handle_signal(int sig)
{
	(void)sig;
	ft_printf("\nExiting...\n");
	cleanup(NULL);
	exit(0);
}

#include <signal.h>

int	main(int argc, char **argv, char **envp)
{
	char		*input;
	t_token		*tokens;
	t_command	*commands;
	t_ctx		*ctx;
	int			status;

	(void)argc;
	(void)argv;
	ctx = init_ctx(envp);
	if (!ctx)
		return (1);
	status = 0;
	while (status == 0)
	{
		input = readline(YELLOW"minishell$ "RESET);
		if (!input)
			break;
		if (input[0] != '\0')
			add_history(input);
		if (ft_strncmp(input, "exit", ft_strlen(input)) == 0)
		{
			free(input);
			break;
		}
		tokens = tokenize(input);
		if (tokens)
		{
			print_tokens(tokens);
			commands = parse_token(tokens, ctx);
			if (commands)
			{
				print_commands(commands);
				free_all_commands(commands);
			}
			free_all_token(tokens);
		}
		free(input);
	}
	free_env_list(ctx->env_list);
	free(ctx);
	return (0);
}





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
// 			break;
// 		if (line[ft_strlen(line) - 1] == '\n')
// 			line[ft_strlen(line) - 1] = '\0';
//
// 		if (process_line(ctx, line))
// 		{
// 			exit_status = ctx->exit_status;
// 			free(line);
// 			break;
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