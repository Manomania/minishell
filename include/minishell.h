/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 17:15:54 by maximart          #+#    #+#             */
/*   Updated: 2025/03/10 11:06:06 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft.h"
# include <errno.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/wait.h>

// *************************************************************************** #
//                                   Macros                                    #
// *************************************************************************** #

# define RESET "\033[039m"
# define RED "\033[091m"
# define GREEN "\033[092m"
# define YELLOW "\033[093m"

// *************************************************************************** #
//                                 Structures                                  #
// *************************************************************************** #

typedef enum e_bool
{
	false,
	true,
}							t_bool;

typedef enum e_error_type
{
	ERR_NONE = 0,
	ERR_CMD_NOT_FOUND,
	ERR_NO_PERMISSION,
	ERR_IO_ERROR,
	ERR_UNIMPLEMENTED,
	ERR_ALLOC,
}							t_error_type;

typedef struct s_error_info
{
	int						code;
	const char				*message;
	t_bool					use_perror;
}							t_error_info;

typedef enum e_token_type
{
	TOK_WORD,          // Commands, args, filename
	TOK_REDIR_FROM,    // <
	TOK_REDIR_TO,      // >
	TOK_HERE_DOC_FROM, // <<
	TOK_HERE_DOC_TO,   // >>
	TOK_PIPE,          // |
	TOK_ENV,           // $
	TOK_AND,           // &&
	TOK_OR,            // ||
	TOK_NEW_LINE,      // \n
	TOK_EOF,           // '\0'
}							t_token_type;

typedef struct s_token
{
	t_token_type			type;
	struct s_token			*next;
	char					*value;
}							t_token;

typedef struct s_lexer
{
	char					*input;
	int						position;
	int						length;
}							t_lexer;

/**
 * @brief Represents a command redirection (input/output)
 */
typedef struct s_redirection
{
	// File descriptor (0 for input, 1 for output)
	int						fd;
	// Target filename
	char					*filename;
	// Redirection type (< > << >>)
	t_token_type			type;
	// Next redirection in the list
	struct s_redirection	*next;
}							t_redirection;

/**
 * @brief Represents a single command with its arguments and redirections
 */
typedef struct s_command
{
	// Command name
	char					*cmd;
	// Array of arguments (including command as [0])
	char					**args;
	// Number of arguments
	int						arg_count;
	// Linked list of redirections
	t_redirection			*redirections;
	// Next command in pipeline
	struct s_command		*next;
	// Previous command in pipeline
	struct s_command		*prev;
}							t_command;

/**
 * @brief Represents a parsed token
 */
typedef struct s_ctx
{
	int						argc;
	char					**argv;
	char					**envp;
	t_token					*tokens;
	t_command				*cmd;
}							t_ctx;

// *************************************************************************** #
//                            Function Prototypes                              #
// *************************************************************************** #

// init_parsing.c
t_lexer						*create_lexer(char *input);
t_token						*create_token(t_token_type type, char *value);

// Token
t_bool						token_is_redirection(t_token_type type);
void						free_token(t_token *token);
void						free_all_token(t_token *token);

// lexer_utils.c
char						get_lexer(t_lexer *lexer);
void						advance_lexer(t_lexer *lexer);
void						skip_whitespace_lexer(t_lexer *lexer);

// parse_line.c
char						*read_quoted_string_lexer(t_lexer *lexer,
								char quote_char);
char						*read_word_lexer(t_lexer *lexer);
t_token						*next_token_lexer(t_lexer *lexer);
t_token						*tokenize(char *input);

// debug.c
void						print_tokens(t_token *tokens);

// ctx_*.c
t_ctx						*ctx_init(int argc, char **argv, char **envp);
void						ctx_clear(t_ctx *ctx);
void						ctx_exit(t_ctx *ctx);

// Commands
int							command_add_redirection(t_command *cmd,
								t_token_type type, int fd, char *filename);
int							handle_redirections(t_redirection *redirections);
int							command_add_argument(t_command *cmd, char *arg);
int							command_execute(t_ctx *ctx, t_command *cmd);
void						command_free(t_command *cmd);
t_command					*command_parse(t_token *tokens);
t_command					*command_new(void);

// Debug
void						print_tokens_list(t_token *tokens);

// ctx_error*.c
void						ctx_error_exit(t_ctx *ctx, t_error_type err);
int							ctx_error(t_error_type err);

// Builtins
t_bool						builtins_try(t_ctx *ctx, t_command *cmd);

// Env vars
char						*env_find(t_ctx *ctx, char *bin);
char						*env_find_bin(t_ctx *ctx, char *bin);

// Paths manipulation
char						*bin_find_path(t_ctx *ctx, char *dir, char *bin);
char						*bin_find(t_ctx *ctx, char *bin);

// Memory
void						free_2d_array(void **ptrs);

// Pipelines
void						execute_command_in_pipeline(t_ctx *ctx,
								t_command *cmd, int pipes[2][2], int cmd_index,
								int cmd_count);
int							create_pipes(int pipes[2][2], int cmd_index,
								int cmd_count);
void						close_previous_pipe(int pipes[2][2], int cmd_index);
int							wait_for_commands(pid_t *pids, int cmd_count);
int							count_commands(t_command *commands);
void						close_all_pipes(int pipes[2][2]);
void						swap_pipes(int pipes[2][2]);

// Signals
void						setup_signals(void);
void						reset_signals(void);

#endif
