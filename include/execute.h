/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 16:43:21 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/05 16:25:47 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXECUTE_H
# define EXECUTE_H

# include "minishell.h"

/**
 * @brief Arguments for handle_command_fork
 */
typedef struct s_hcf_args
{
	t_command	*current;
	int			prev_pipe;
	int			*pipe_fds;
	pid_t		*pids;
	int			i;
}				t_hcf_args;

/**
 * @brief Arguments for execute_child_command
 */
typedef struct s_ecc_args
{
	t_command	*cmd;
	int			prev_pipe;
	int			*pipe_fds;
	int			*pids;
}				t_ecc_args;

/**
 * @brief Arguments for execute_and_cleanup
 */
typedef struct s_eac_args
{
	t_command	*current;
	int			prev_read;
	int			pipe_fds[2];
	pid_t		*pids;
	int			i;
}				t_eac_args;

// execute_commands.c
void			execute_commands(t_ctx *ctx, t_command *cmd);

// execute_builtin.c
int				execute_builtin(t_ctx *ctx, t_command *cmd);
t_bool			is_builtin_command(char *cmd_name);

// execute_pipeline.c
void			execute_pipeline(t_ctx *ctx, t_command *cmd);

// execute_pipeline_clean.c
void			cleanup_pipes(int prev_pipe_read, int pipe_fds[2],
					int has_next);
void			child_process_exit(t_ctx *ctx, int *pids, int status);

// execute_pipeline_command.c
void			execute_external_command(t_ctx *ctx, t_command *cmd, int *pids);
void			execute_command_in_child(t_ctx *ctx, t_command *cmd, t_fds fds,
					int *pids);

// execute_pipeline_fork.c
int				handle_command_fork(t_ctx *ctx, t_hcf_args a);
void			handle_fork_error(pid_t *pids, int cmd_count, int pipe_fds[2],
					int prev_pipe_read);

// execute_pipeline_redirs.c
void			handle_fd_redirection(t_fds fds);

// execute_pipeline_utils.c
int				count_commands(t_command *cmd);
int				setup_command_pipe(int pipe_fds[2], t_bool has_next);

// heredoc.c
int				read_all_heredocs(t_ctx *ctx);

// heredoc_expand.c
char			*expand_variables_in_line(t_ctx *ctx, char *line);

// heredoc_expand_utils.c
char			*init_expansion(char *line);
char			*extract_var_name(char *str, int start, int end);

// heredoc_process.c
int				read_heredoc_content(int *pipe_fds, char *delimiter,
					t_ctx *ctx);

// heredoc_signals
void			reset_heredoc_state(void);
t_bool			is_heredoc_interrupted(void);
int				interrupt_check_hook(void);

// heredoc_utils.c
char			*replace_substring(char *str, int start, int end,
					char *replacement);

// heredoc_utils2.c
int				read_heredoc_line(char *delimiter, char **line);

// redir_cleanup.c
void			cleanup_heredoc_resources(t_ctx *ctx);

// redirection.c
t_bool			apply_redirections(t_command *cmd);
t_bool			save_original_fds(int *stdin_fd, int *stdout_fd);
void			restore_original_fds(int stdin_fd, int stdout_fd);

// redirection_apply_output.c
t_bool			apply_output_redirection(t_command *cmd);

// redirection_apply_input.c
t_bool			apply_input_redirection(t_command *cmd);

// free_command.c
void			free_command(t_command *cmd);
void			free_all_commands(t_command *cmd);

#endif
