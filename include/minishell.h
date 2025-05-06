/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 17:15:54 by maximart          #+#    #+#             */
/*   Updated: 2025/05/06 13:52:14 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft.h"
# include <errno.h>
# include <fcntl.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>

// *************************************************************************** #
//                                   Macros                                    #
// *************************************************************************** #

# define RESET "\033[039m"
# define RED "\033[091m"
# define BLUE "\033[034m"
# define GREEN "\033[092m"
# define YELLOW "\033[093m"
# define CYAN "\033[36m"

// *************************************************************************** #
//                                 Structures                                  #
// *************************************************************************** #

typedef enum e_bool
{
	false,
	true,
}							t_bool;

typedef enum e_token_type
{
	TOK_NONE,
	TOK_WORD,
	TOK_REDIR_FROM,
	TOK_REDIR_TO,
	TOK_HERE_DOC_FROM,
	TOK_HERE_DOC_TO,
	TOK_PIPE,
	TOK_OR,
	TOK_AND,
	TOK_ESP,
	TOK_NEW_LINE,
	TOK_EOF,
}							t_token_type;

typedef struct s_quote_state
{
	t_bool					in_single_quote;
	t_bool					in_double_quote;
}							t_quote_state;

typedef struct s_token
{
	t_token_type			type;
	t_quote_state			quote;
	struct s_token			*next;
	char					*value;
}							t_token;

typedef struct s_lexer
{
	t_quote_state			quote;
	t_bool					status;
	char					*input;
	int						position;
	int						length;
}							t_lexer;

/**
 * @brief Represents a command redirection (input/output)
 */
typedef struct s_redirection
{
	t_token_type			type;
	struct s_redirection	*next;
	int						fd;
	char					*filename;
}							t_redirection;

/**
 * @brief Represents a single command with its arguments and redirections
 */
typedef struct s_command
{
	t_token_type			operator;
	t_redirection			*redirection;
	struct s_command		*next;
	char					**args;
	int						arg_count;
}							t_command;

typedef struct s_parse
{
	t_token					*token;
	t_token					*current;
}							t_parse;

typedef struct s_env
{
	struct s_env			*next;
	char					*key;
	char					*value;
}							t_env;

typedef struct s_fds
{
	int						in;
	int						out;
}							t_fds;

typedef enum e_error_type
{
	ERR_CMD_NOT_FOUND,
	// Fs
	ERR_NO_PERMS,
	ERR_NO_FILE,
	ERR_IS_DIR,
	ERR_NO_PWD,
	// Env
	ERR_NO_OLDPWD,
	ERR_NO_HOME,
	// Validation - input
	ERR_VLD_INPUT_LENGTH,
	// Validation - environment
	ERR_VLD_ENV_VAR_EMPTY,
	ERR_VLD_ENV_VAR_START,
	ERR_VLD_ENV_VAR_BAD_CHAR,
	ERR_VLD_ENV_VAR_TOO_LONG,
	// Validation - redirection
	ERR_VLD_REDIR_FILENAME_EMPTY,
	ERR_VLD_REDIR_FILENAME_TOO_LONG,
	// Lexer
	ERR_UNCLOSED_QUOTE,
	ERR_TOKEN_LIST,
	// Other
	ERR_IO,
	ERR_FD,
	ERR_ALLOC,
	ERR_CHILD,
	ERR_PIPE,
	ERR_IDENTIFIER,
	ERR_NUMERIC,
	ERR_TOO_MANY_ARGS,
}							t_error_type;

typedef struct s_error_info
{
	int						code;
	const char				*message;
}							t_error_info;

/**
 * @brief Represents a parsed token
 */
typedef struct s_ctx
{
	t_quote_state			quote;
	t_env					*env_list;
	int						argc;
	char					**argv;
	char					**envp;
	t_token					*tokens;
	t_command				*cmd;
	int						exit_status;
	t_bool					exit_requested;
	int						fd_file_in;
	int						fd_file_out;
}							t_ctx;

/**
 * @brief Simple enum proper error handling in bin_find.c
 */
typedef enum e_path_error
{
	PATH_ERR_NONE,
	PATH_ERR_NOT_FOUND,
	PATH_ERR_NO_PERMISSION,
	PATH_ERR_IS_DIR,
	PATH_ERR_OTHER
}							t_path_error;

/**
 * @brief Helper struct to track position data during processing.
 * @note Used in env management
 */
typedef struct s_pos
{
	int						start;
	int						current;
}							t_pos;

typedef struct s_handle_token
{
	t_token					**current;
	t_bool					*first_arg_processed;
	t_bool					*has_redirections;
}							t_handle_token;

// *************************************************************************** #
//                            Function Prototypes                              #
// *************************************************************************** #

// parser_utils.c
char						*get_token_value(t_parse *parse);
int							check_parse(t_parse *parse, t_token_type type);
int							consume_parse(t_parse *parse, t_token_type type);
int							check_token_type(t_parse *parse, t_token_type type);
void						advance_parse(t_parse *parse);

// parser_command.c
t_command					*parse_command(t_parse *parse, t_ctx *ctx);
int							parse_redirection(t_parse *parse, t_command *cmd,
								t_ctx *ctx);
void						add_redirection(t_command *cmd,
								t_redirection *redirection);

// parser_command_utils.c
int							add_argument(t_command *cmd, char *value);

#endif
