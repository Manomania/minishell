/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 17:15:54 by maximart          #+#    #+#             */
/*   Updated: 2025/03/14 15:44:27 by elagouch         ###   ########.fr       */
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
	TOK_WORD,
	TOK_REDIR_FROM,
	TOK_REDIR_TO,
	TOK_HERE_DOC_FROM,
	TOK_HERE_DOC_TO,
	TOK_PIPE,
	TOK_ENV,
	TOK_AND,
	TOK_OR,
	TOK_NEW_LINE,
	TOK_EOF,
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

typedef enum e_error_type
{
	ERR_NONE = 0,
	ERR_CMD_NOT_FOUND,
	ERR_NO_PERMISSION,
	ERR_IO_ERROR,
	ERR_UNIMPLEMENTED,
	ERR_ALLOC,
	ERR_PIPE,
	ERR_CHILD,
}							t_error_type;

typedef struct s_error_info
{
	int						code;
	const char				*message;
	t_bool					use_perror;
}							t_error_info;

/**
 * @brief Represents a parsed token
 */
typedef struct s_ctx
{
	t_env					*env_list;
	int						exit_status;
	int						interactive;
	int						argc;
	char					**argv;
	char					**envp;
	t_token					*tokens;
	t_command				*cmd;
	int						fd_file_in;
	int						fd_file_out;
}							t_ctx;

/**
 * @brief Structure to hold pipeline process data
 */
typedef struct s_pipe_data
{
	t_command				*current;
	int						cmd_count;
	int						i;
	int						prev_pipe;
	pid_t					*pids;
	int						pipe_fds[2];
}							t_pipe_data;

// *************************************************************************** #
//                            Function Prototypes                              #
// *************************************************************************** #

// init.c
t_ctx						*init_ctx(char **envp);
t_env						*create_env_node(char *key, char *value);
int							add_env_var(t_env **env_list, char *key,
								char *value);

// init_parse.c
t_command					*create_command(void);
t_lexer						*create_lexer(char *input);
t_token						*create_token(t_token_type type, char *value);
t_redirection				*create_redirection(t_token_type type,
								char *filename);
void						init_parse_context(t_parse *parse, t_token *token);

// lexer_utils.c
char						get_lexer(t_lexer *lexer);
void						advance_lexer(t_lexer *lexer);
void						skip_whitespace_lexer(t_lexer *lexer);

// lexer_read.c
t_token						*tokenize(char *input);
char						*read_word_lexer(t_lexer *lexer);
char						*read_complex_word(t_lexer *lexer);
char						*read_quoted_string_lexer(t_lexer *lexer,
								char quote_char);

// lexer_token.c
t_token						*next_token_lexer(t_lexer *lexer);
void						free_token(t_token *token);
void						free_all_token(t_token *token);

// parser_utils.c
void						advance_parse(t_parse *parse);
char						*get_token_value(t_parse *parse);
int							check_parse(t_parse *parse, t_token_type type);
int							consume_parse(t_parse *parse, t_token_type type);
int							check_token_type(t_parse *parse, t_token_type type);

// parser_command.c
int							add_argument(t_command *cmd, char *value);
void						add_redirection(t_command *cmd,
								t_redirection *redirection);
int							parse_redirection(t_parse *parse, t_command *cmd);
t_command					*parse_command(t_parse *parse);
t_command					*parse_token(t_token *token);

// parser_pipeline.c
t_command					*parse_pipeline(t_parse *parse);
t_command					*parse_command_sequence(t_parse *parse);
void						connect_commands(t_command *left_cmd,
								t_command *right_cmd, t_token_type op_type);

// free.c
void						free_redirection(t_redirection *redirection);
void						free_all_redirection(t_redirection *redirection);
void						free_command_pipeline(t_command *cmd);
void						free_command(t_command *cmd);
void						free_all_commands(t_command *cmd);

// free_env.c
void						free_env_list(t_env *env_list);
int							parse_env_var(char *env_str, t_env **env_list);

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
void						command_free(t_command *cmd);
int							command_execute(t_ctx *ctx);
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
char						*bin_find_path(t_ctx *ctx, const char *dir,
								char *bin);
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

// Execution
int							get_fd_in(t_ctx *ctx, t_list *fd_pipes,
								int cmd_index);
int							get_fd_out(t_ctx *ctx, t_list *fd_pipes,
								int cmd_index, int cmd_count);
void						exec_cmda_child(t_ctx *ctx, t_command *current_cmd,
								int *pipe_prev, int *pipe_curr);
void						exec_cmda_parent(t_command **current_cmd,
								int *pipe_prev, int *pipe_curr);
t_bool						command_bin(t_ctx *ctx);
t_bool						exec_cmdas(t_ctx *ctx);

// Execution: utils
void						execute_command(t_ctx *ctx, t_command *cmd);
void						setup_child_process(t_ctx *ctx, t_command *cmd,
								int input_fd, int output_fd);
pid_t						exec_piped_command(t_ctx *ctx, t_command *cmd,
								int input_fd, int output_fd);
int							setup_pipe(int pipe_fds[2]);
int							wait_for_pids(pid_t *pids, int count);

// signals
void						setup_signals(void);
void						reset_signals(void);
void						setup_parent_signals(void);
void						set_child_mode(int in_child);

#endif
