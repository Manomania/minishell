/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 17:15:54 by maximart          #+#    #+#             */
/*   Updated: 2025/04/23 12:58:33 by elagouch         ###   ########.fr       */
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

typedef struct s_token
{
	t_token_type			type;
	struct s_token			*next;
	char					*value;
}							t_token;

typedef struct s_quote_state
{
	int						in_single_quote;
	int						in_double_quote;
}							t_quote_state;

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
	t_bool					status;
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
	t_bool					debug;
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
 * @brief Helper struct to track position data during processing
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

extern int	g_signal_status;

// token_checker.c
t_bool						validate_token_sequence(t_token *tokens);

// token_checker_utils.c
t_bool						check_pipe_tokens(t_token *current);
t_bool						check_redir_combinations(t_token *current);
t_bool						check_logical_tokens(t_token *token);

// command_add.c
int							command_add_argument(t_command *cmd, char *arg);
int							command_add_redirection(t_command *cmd,
								t_token_type type, char *filename);

// command_bin.c
t_bool						command_bin(t_ctx *ctx);

// command_execute.c
int							command_execute(t_ctx *ctx);
int							setup_command_redirections(t_ctx *ctx,
								t_command *cmd);
int							validate_cmd_in_child(t_ctx *ctx);

// command_execute_utils.c
int							get_exit_status(int status);
int							handle_child_redirections(t_ctx *ctx);

// command_execute_utils2.c
t_bool						has_only_redirections(t_command *cmd);
int							execute_redirections_only(t_ctx *ctx);
void						cleanup_child_process(t_ctx *ctx);
int							read_all_heredocs(t_ctx *ctx);

// command_execute_utils3.c
void						execute_child(t_ctx *ctx);

// command_execute_utils4.c
char						**create_env_array(t_env *env_list);

// command_execute_utils5.c
int							execute_single_command(t_ctx *ctx);
int							process_signal_exit(int status);
void						close_standard_io(int saved_stdin,
								int saved_stdout);
void						handle_standard_io(int saved_stdin,
								int saved_stdout);
void						execute_child_process(t_ctx *ctx, int saved_stdin,
								int saved_stdout);

// command_execute_utils6.c
int							handle_parent_process(pid_t pid, int saved_stdin,
								int saved_stdout);
int							execute_external_command(t_ctx *ctx,
								int saved_stdin, int saved_stdout);
int							process_command_type(t_ctx *ctx);

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
t_bool						process_word_token_case(t_command *cmd,
								t_token **current, t_ctx *ctx,
								t_bool *first_arg_processed);

// command_parse_utils2.c
int							handle_redirection_token(t_command *cmd,
								t_token *token, t_token *next_token,
								t_ctx *ctx);
t_bool						handle_empty_first_arg(t_command *cmd,
								t_token **current, t_ctx *ctx);

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
char						*get_env_value(t_env *env_list, char *key);
char						*get_var_name(char *str, int *pos);
char						*expand_special_var(t_ctx *ctx, char *str, int *i);

// env_utils.c
char						*expand_positional_param(char *str, int *i);
char						*expand_env_var(t_ctx *ctx, char *str, int *i);
char						*expand_variable(t_ctx *ctx, char *str, int *i);

// env_dupe.c
t_env						*duplicate_env_list(t_env *original_list);

// env_find.c
char						*env_find(t_ctx *ctx, char *var);

// env_find_bin.c
char						*env_find_bin(t_ctx *ctx, char *bin);

// env_quotes.c
char						*handle_quotes_and_vars(t_ctx *ctx, char *str);
char						*append_text_part(int start, int i, char *result,
								char *str);
char						*handle_var_expansion(t_ctx *ctx, char *str, int *i,
								char *result);

// env_quotes_utils.c
char						*process_string(t_ctx *ctx, char *str,
								char *result);

// exec_cmdas.c
int							handle_descriptors(int prev_pipe, int pipe_fds[2],
								int i, int cmd_count);
int							handle_pipe_setup(int pipe_fds[2], int i,
								int cmd_count);
void						cleanup_pipe_fds(int pipe_fds[2]);
pid_t						execute_pipeline_cmd_with_redir(t_ctx *ctx,
								t_pipe_data *data, int input_fd, int output_fd);

// exec_cmdas_utils.c
int							setup_pipe(int pipe_fds[2]);
int							wait_for_pids(pid_t *pids, int count);
t_bool						init_pipe_data(t_pipe_data *data, t_ctx *ctx);
int							wait_for_pipeline_processes(pid_t *pids, int count);

// exec_cmdas_utils2.c
int							count_commands(t_command *cmd);
void						setup_child_process(t_ctx *ctx, t_command *cmd,
								int input_fd, int output_fd);
pid_t						execute_redirections_only_pipeline(t_ctx *ctx,
								t_pipe_data *data);
void						execute_command(t_ctx *ctx, t_command *cmd);
void						report_cmd_not_found_pipe(char *cmd_name);

// exec_cmdas_utils3.c
t_bool						check_command_binary(t_ctx *ctx, t_pipe_data *data);
t_bool						validate_pipeline_command(t_pipe_data *data);
int							handle_non_builtin(t_ctx *ctx, t_pipe_data *data);
t_bool						has_only_redirections_pipeline(t_command *command);

// exec_cmdas_utils4.c
int							prepare_all_pipeline_files(t_command *cmd);
int							setup_child_pipeline_redirections(t_command *cmd,
								int input_fd, int output_fd);

// exec_cmdas_utils5.c
void						handle_signal_output(int status);
int							process_last_command_status(int status,
								int *was_signaled);
void						setup_pipe_redirections(t_pipe_data *data);
char						*validate_and_resolve_command(t_ctx *ctx,
								t_command *cmd);

// exec_cmdas_utils6.c
int							exec_cmdas(t_ctx *ctx);

// exec_cmdas_utils7.c
int							setup_child_redirections_bruh(t_ctx *ctx,
								t_command *current, int prev_pipe,
								int next_write);
void						handle_child_process(t_ctx *ctx,
								t_command *current);

// ctx_exit.c
void						ctx_exit(t_ctx *ctx);

// free_2d_array.c
void						free_2d_array(void **ptrs);

// free_commands.c
void						free_command(t_command *cmd);
void						free_all_commands(t_command *cmd);

// free_ctx.c
void						ctx_clear(t_ctx *ctx);
void						final_cleanup(t_ctx *ctx);

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

// lexer_read_utils2.c
void						set_quote_flags(t_lexer *lexer, char quote_char);
char						*create_quoted_content(t_lexer *lexer, int start,
								int end);

// lexer_token.c
t_token						*next_token_lexer(t_lexer *lexer);
void						free_all_token(t_token *token);

// lexer_token_is.c
t_bool						token_is_redirection(t_token_type type);

// lexer_token_utils.c
t_token						*handle_basics_token(t_lexer *lexer);
t_token						*handle_pipe_and_token(t_lexer *lexer);
t_token						*handle_redir_from_and_to_token(t_lexer *lexer);
// t_token						*handle_env_token(t_lexer *lexer);

// lexer_tokenize.c
t_token						*tokenize(t_ctx *ctx, char *input);

// lexer_utils.c
char						get_lexer(t_lexer *lexer);
void						advance_lexer(t_lexer *lexer);
void						skip_whitespace_lexer(t_lexer *lexer);
void						sync_quote_state(t_ctx *ctx, t_lexer *lexer);

// parser_utils.c
char						*get_token_value(t_parse *parse);
int							check_parse(t_parse *parse, t_token_type type);
int							consume_parse(t_parse *parse, t_token_type type);
int							check_token_type(t_parse *parse, t_token_type type);
void						advance_parse(t_parse *parse);

// bin_find.c
char						*bin_find(t_ctx *ctx, char *bin);
char						*resolve_relative_path(char *bin);
t_bool						is_directory(const char *path);

// bin_find_utils.c
char						*check_relative_path(char *bin,
								t_path_error *error_state);
void						display_path_error(char *bin,
								t_path_error error_state);

// bin_find_path.c
char						*bin_find_path(const char *dir, char *bin);

// heredoc.c
int							setup_heredocs(t_ctx *ctx, t_command *cmd);
int							create_heredoc(t_ctx *ctx, char *delimiter);

// heredoc_utils.c
char						*replace_substring(char *str, int start, int end,
								char *replacement);

// heredoc_utils2.c
int							read_heredoc_line(char *delimiter, char **line);
int							setup_heredoc_pipes(int pipe_fds[2]);
int							wait_heredoc_child(int pipe_fds[2], t_ctx *ctx);

// heredoc_expand.c
char						*expand_variables_in_line(t_ctx *ctx, char *line);

// heredoc_expand_utils.c
char						*init_expansion(char *line);
char						*extract_var_name(char *str, int start, int end);

// redir_cleanup.c
void						cleanup_heredoc_resources(t_ctx *ctx);

// redirections.c
int							setup_redirections(t_redirection *redirections);

// signals.c
void						setup_signals(void);
void						reset_signals(void);
void						setup_parent_signals(void);
void						update_signal_status(t_ctx *ctx);

// parser_command.c
t_command					*parse_command(t_parse *parse, t_ctx *ctx);
int							parse_redirection(t_parse *parse, t_command *cmd,
								t_ctx *ctx);
void						add_redirection(t_command *cmd,
								t_redirection *redirection);
char						*create_prompt(int prev_status);

// parser_command_utils.c
int							add_argument(t_command *cmd, char *value);

// main.c
// A REMPLIR

// main_utils.c
t_bool						parse_user_input(t_ctx *ctx, char *input);
void						process_command(t_ctx *ctx);

// main_utils2.c
char						*get_user_input(t_ctx *ctx, int prev_status);
void						handle_command_in_main_loop(t_ctx *ctx,
								char *input);

#endif
