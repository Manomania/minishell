/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 17:15:54 by maximart          #+#    #+#             */
/*   Updated: 2025/03/24 15:38:54 by elagouch         ###   ########.fr       */
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
# define MAGENTA "\033[35m"
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
	TOK_OPEN_PAR,
	TOK_CLOSE_PAR,
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
	ERR_NO_SUCH_FILE,
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
	t_bool					exit_requested;
	int						interactive;
	int						argc;
	char					**argv;
	char					**envp;
	t_token					*tokens;
	t_command				*cmd;
	int						fd_file_in;
	int						fd_file_out;
}							t_ctx;

typedef struct s_quote_state
{
	int						in_single_quote;
	int						in_double_quote;
}							t_quote_state;

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

/**
 * @brief Simple enum proper error handling in bin_find.c
 */
typedef enum e_path_error
{
	PATH_ERR_NONE,
	PATH_ERR_NOT_FOUND,
	PATH_ERR_NO_PERMISSION,
	PATH_ERR_OTHER
}							t_path_error;

// *************************************************************************** #
//                            Function Prototypes                              #
// *************************************************************************** #

// builtins_try.c
t_bool						builtins_try(t_ctx *ctx, t_command *cmd);

// command_add.c
int							command_add_argument(t_command *cmd, char *arg);
int							command_add_redirection(t_command *cmd,
								t_token_type type, int fd, char *filename);

// command_bin.c
t_bool						command_bin(t_ctx *ctx);

// command_execute.c
int							command_execute(t_ctx *ctx);

// command_execute_utils.c
int							get_exit_status(int status);
void						execute_child(t_ctx *ctx);

// command_new.c
t_command					*command_new(void);

// command_parse.c
t_command					*command_parse(t_ctx *ctx, t_token *tokens);

// command_parse_utils.c
t_bool						handle_first_arg(t_command *cmd,
								char *expanded_value);
t_bool						add_to_existing_args(t_command *cmd,
								char *expanded_value);
t_bool						process_word_token(t_command *cmd, t_token *token,
								t_ctx *ctx);

// command_redirection.c
int							handle_redirections(t_redirection *redirections);

// debug_utils.c
void						print_tokens(t_token *tokens);

// debug_utils2.c
char						*get_token_type_str(t_token_type type);
void						debug_exit_status(t_ctx *ctx);
void						print_token(t_token *current, int token_count);
void						print_tokens_list(t_token *tokens);
void						print_redirection_type(t_token_type type);

// env.c
char						*expand_var(t_ctx *ctx, char *var_name);
char						*append_part(char *result, char *str, int start,
								int end);
char						*expand_variable(t_ctx *ctx, char *str, int *i,
								int in_squote);

// env_find.c
char						*env_find(t_ctx *ctx, char *var);

// env_find_bin.c
char						*env_find_bin(t_ctx *ctx, char *bin);

// env_quotes
char						*handle_quotes_and_vars(t_ctx *ctx, char *str);

// exec_cmdas.c
int							handle_descriptors(int prev_pipe, int pipe_fds[2],
								int i, int cmd_count);
int							exec_cmdas(t_ctx *ctx);

// exec_cmdas_utils.c
int							setup_pipe(int pipe_fds[2]);
int							wait_for_pids(pid_t *pids, int count);
t_bool						init_pipe_data(t_pipe_data *data, t_ctx *ctx);
int							wait_for_pipeline_processes(pid_t *pids, int count);

// exec_cmdas_utils2.c
int							count_commands(t_command *cmd);
void						setup_child_process(t_ctx *ctx, t_command *cmd,
								int input_fd, int output_fd);
void						debug_exit_status_cmdas(int exit_status);

// exec_cmdas_utils3.c
t_bool						check_command_binary(t_ctx *ctx, t_pipe_data *data);
t_bool						validate_pipeline_command(t_pipe_data *data);
int							handle_non_builtin(t_ctx *ctx, t_pipe_data *data);

// ctx_exit.c
void						ctx_exit(t_ctx *ctx);

// free_2d_array.c
void						free_2d_array(void **ptrs);

// free_commands.c
void						free_command(t_command *cmd);
void						free_all_commands(t_command *cmd);

// free_ctx.c
void						ctx_clear(t_ctx *ctx);

// free_env.c
void						free_env_list(t_env *env_list);
int							parse_env_var(char *env_str, t_env **env_list);

// init_ctx.c
t_ctx						*init_ctx(int argc, char **argv, char **envp);
int							add_env_var(t_env **env_list, char *key,
								char *value);

// init_parse.c
t_command					*create_command(void);
t_token						*create_token(t_token_type type, char *value);
t_redirection				*create_redirection(t_token_type type,
								char *filename);
void						init_parse_context(t_parse *parse, t_token *token);

// lexer_read.c
char						*read_word_lexer(t_lexer *lexer);
char						*read_complex_word(t_lexer *lexer);
char						*read_quoted_string_lexer(t_lexer *lexer,
								char quote_char);

// lexer_read_utils.c
char						*join_and_free(char *s1, char *s2);
char						*handle_dollar_sign(char *result);
char						*handle_word_part(t_lexer *lexer, char *result);
char						*handle_quoted_part(t_lexer *lexer, char *result,
								char quote_char);

// lexer_token.c
t_token						*next_token_lexer(t_lexer *lexer);
void						free_all_token(t_token *token);

// lexer_token_is.c
t_bool						token_is_redirection(t_token_type type);

// lexer_token_utils.c
t_token						*handle_basics_token(t_lexer *lexer);
t_token						*handle_pipe_and_token(t_lexer *lexer);
t_token						*handle_redir_from_and_to_token(t_lexer *lexer);
t_token						*handle_env_token(t_lexer *lexer);

// lexer_tokenize.c
t_token						*tokenize(t_ctx *ctx, char *input);

// lexer_utils.c
char						get_lexer(t_lexer *lexer);
void						advance_lexer(t_lexer *lexer);
void						skip_whitespace_lexer(t_lexer *lexer);

// parser_utils.c
char						*get_token_value(t_parse *parse);
int							check_parse(t_parse *parse, t_token_type type);
int							consume_parse(t_parse *parse, t_token_type type);
int							check_token_type(t_parse *parse, t_token_type type);
void						advance_parse(t_parse *parse);

// bin_find.c
char						*bin_find(t_ctx *ctx, char *bin);
char						*resolve_relative_path(char *bin);

// bin_find_utils.c
char						*check_relative_path(char *bin,
								t_path_error *error_state);
void						display_path_error(char *bin,
								t_path_error error_state);

// bin_find_path.c
char						*bin_find_path(const char *dir, char *bin);

// heredoc.c
int							setup_heredocs(t_ctx *ctx, t_command *cmd);

// heredoc_utils.c
char						*replace_substring(char *str, int start, int end,
								char *replacement);

// heredoc_expand.c
char						*expand_variables_in_line(t_ctx *ctx, char *line);

// heredoc_expand_utils.c
char						*init_expansion(char *line);
char						*extract_var_name(char *str, int start, int end);

// redirections.c
int							setup_redirections(t_redirection *redirections);

// signals.c
void						setup_signals(void);
void						reset_signals(void);
void						setup_parent_signals(void);

// parser_command.c
t_command					*parse_command(t_parse *parse, t_ctx *ctx);
int							parse_redirection(t_parse *parse, t_command *cmd,
								t_ctx *ctx);
void						add_redirection(t_command *cmd,
								t_redirection *redirection);

// parser_command_utils.c
int							add_argument(t_command *cmd, char *value);

// main.c
// A REMPLIR

// main_utils.c
char						*get_user_input(t_ctx *ctx, int prev_status);
int							handle_command_in_main_loop(t_ctx *ctx, int status,
								char *input);

// // parser_pipeline.c
// t_command		*parse_pipeline(t_ctx *ctx, t_parse *parse);
// t_command		*parse_command_sequence(t_ctx *ctx, t_parse *parse);

#endif
