/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 17:15:54 by maximart          #+#    #+#             */
/*   Updated: 2025/03/05 11:32:25 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft.h"
# include <stdlib.h>
# include <string.h>
# include <stdio.h>
# include <readline/readline.h>
# include <readline/history.h>

/*******************************************************************************
*                                    Macros                                    *
*******************************************************************************/

# define RESET	"\033[039m"
# define RED	"\033[091m"
# define BLUE	"\033[034m"
# define GREEN	"\033[092m"
# define YELLOW	"\033[093m"

/*******************************************************************************
*                                  Structures                                  *
*******************************************************************************/

typedef enum e_token_type
{
	TOK_WORD,			// Commands, args, filename
	TOK_REDIR_FROM,		// <
	TOK_REDIR_TO,		// >
	TOK_HERE_DOC_FROM,	// <<
	TOK_HERE_DOC_TO,	// >>
	TOK_PIPE,			// |
	TOK_ENV,			// $
	TOK_AND,			// &&
	TOK_OR,				// ||
	TOK_NEW_LINE,		// \n
	TOK_EOF,			// '\0'
}	t_token_type;

typedef struct s_token
{
	t_token_type	type;
	struct s_token	*next;
	char			*value;
}					t_token;

typedef struct s_lexer
{
	char	*input;
	int		position;
	int		length;
}			t_lexer;

typedef struct s_redirection
{
	t_token_type			type;
	struct s_redirection	*next;
	char					*filename;
}							t_redirection;

typedef	struct s_command
{
	t_token_type		operator;
	t_redirection		*redirection;
	struct s_command	*next;
	char				**args;
	int					arg_count;
}						t_command;

typedef struct s_parse
{
	t_token	*token;
	t_token	*current;
}			t_parse;

typedef struct s_env
{
	struct s_env	*next;
	char			*key;
	char			*value;
}					t_env;

typedef struct s_ctx
{
	t_env	*env_list;
	int		exit_status;
	int		interactive;
}			t_ctx;

/*******************************************************************************
*                             Function Prototypes                              *
*******************************************************************************/

// init.c
t_ctx			*init_ctx(char **envp);
t_env			*create_env_node(char *key, char *value);
int				add_env_var(t_env **env_list, char *key, char *value);

// init_parse.c
t_command		*create_command(void);
t_lexer			*create_lexer(char *input);
t_token			*create_token(t_token_type type, char *value);
t_redirection	*create_redirection(t_token_type type, char *filename);
void			init_parse_context(t_parse *parse, t_token *token);

// lexer_utils.c
char			get_lexer(t_lexer *lexer);
void			advance_lexer(t_lexer *lexer);
void			skip_whitespace_lexer(t_lexer *lexer);

// lexer_read.c
t_token			*tokenize(char *input);
char			*read_word_lexer(t_lexer *lexer);
char			*read_quoted_string_lexer(t_lexer *lexer, char quote_char);

// lexer_token.c
t_token			*next_token_lexer(t_lexer *lexer);
void			free_token(t_token *token);
void			free_all_token(t_token *token);

//parser_utils.c
void			advance_parse(t_parse *parse);
char			*get_token_value(t_parse *parse);
int				check_parse(t_parse *parse, t_token_type type);
int				consume_parse(t_parse *parse, t_token_type type);
int				check_token_type(t_parse *parse, t_token_type type);

//parser_command.c
int				add_argument(t_command *cmd, char *value);
void			add_redirection(t_command *cmd, t_redirection *redirection);
int				parse_redirection(t_parse *parse, t_command *cmd);
t_command		*parse_command(t_parse *parse);
t_command		*parse_token(t_token *token);

//parser_pipeline.c
t_command		*parse_pipeline(t_parse *parse);
t_command		*parse_command_sequence(t_parse *parse);
void			connect_commands(t_command *left_cmd, t_command *right_cmd, t_token_type op_type);

// free.c
void			free_redirection(t_redirection *redirection);
void			free_all_redirection(t_redirection *redirection);
void			free_command_pipeline(t_command *cmd);
void			free_command(t_command *cmd);
void			free_all_commands(t_command *cmd);

// free_env.c
void			free_env_list(t_env *env_list);
int				parse_env_var(char *env_str, t_env **env_list);
void			free_ctx(t_ctx *ctx);

#endif
