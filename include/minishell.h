/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 17:15:54 by maximart          #+#    #+#             */
/*   Updated: 2025/03/08 12:21:39 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft.h"
# include <errno.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>

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
}					t_bool;

typedef enum e_error_type
{
	ERR_NONE = 0,
	ERR_CMD_NOT_FOUND,
	ERR_NO_PERMISSION,
	ERR_IO_ERROR,
	ERR_UNIMPLEMENTED,
}					t_error_type;

typedef struct s_error_info
{
	int				code;
	const char		*message;
	t_bool			use_perror;
}					t_error_info;

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
}					t_token_type;

typedef struct s_token
{
	t_token_type	type;
	struct s_token	*next;
	char			*value;
}					t_token;

typedef struct s_lexer
{
	char			*input;
	int				position;
	int				length;
}					t_lexer;

typedef struct s_ctx
{
	t_token			*tokens;
}					t_ctx;

// *************************************************************************** #
//                            Function Prototypes                              #
// *************************************************************************** #

// init_parsing.c
t_lexer				*create_lexer(char *input);
t_token				*create_token(t_token_type type, char *value);

// token_utils.c
void				free_token(t_token *token);
void				free_all_token(t_token *token);

// lexer_utils.c
char				get_lexer(t_lexer *lexer);
void				advance_lexer(t_lexer *lexer);
void				skip_whitespace_lexer(t_lexer *lexer);

// parse_line.c
char				*read_quoted_string_lexer(t_lexer *lexer, char quote_char);
char				*read_word_lexer(t_lexer *lexer);
t_token				*next_token_lexer(t_lexer *lexer);
t_token				*tokenize(char *input);

// debug.c
void				print_tokens(t_token *tokens);

// ctx_*.c
void				ctx_clear(t_ctx *ctx);
void				ctx_exit(t_ctx *ctx);
t_ctx				*ctx_init(void);

// Commands
void				cmds_handle(t_ctx *ctx);

// Debug
void				print_tokens_list(t_token *tokens);

// ctx_error*.c
void				ctx_error_exit(t_ctx *ctx, t_error_type err);
int					ctx_error(t_error_type err);

// Builtins
t_bool				builtins_try(t_ctx *ctx, t_token *tok);

#endif
