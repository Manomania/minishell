/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 16:43:21 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/05 19:02:23 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXECUTE_H
# define EXECUTE_H

# include "minishell.h"

// execute_commands.c
void	execute_commands(t_ctx *ctx, t_command *cmd);

// execute_builtin.c
int		execute_builtin(t_ctx *ctx, t_command *cmd);
t_bool	is_builtin_command(char *cmd_name);

// execute_pipeline.c
void	execute_pipeline(t_ctx *ctx, t_command *cmd);

// heredoc.c
int		read_all_heredocs(t_ctx *ctx);

// heredoc_expand.c
char	*expand_variables_in_line(t_ctx *ctx, char *line);

// heredoc_expand_utils.c
char	*init_expansion(char *line);
char	*extract_var_name(char *str, int start, int end);

// heredoc_process.c
int		read_heredoc_content(int *pipe_fds, char *delimiter, t_ctx *ctx);

// heredoc_signals
void	reset_heredoc_state(void);
t_bool	is_heredoc_interrupted(void);
int		interrupt_check_hook(void);

// heredoc_utils.c
char	*replace_substring(char *str, int start, int end, char *replacement);

// heredoc_utils2.c
int		read_heredoc_line(char *delimiter, char **line);

// redir_cleanup.c
void	cleanup_heredoc_resources(t_ctx *ctx);

// redirection.c
t_bool	apply_redirections(t_command *cmd);
t_bool	save_original_fds(int *stdin_fd, int *stdout_fd);
void	restore_original_fds(int stdin_fd, int stdout_fd);

// free_command.c
void	free_command(t_command *cmd);
void	free_all_commands(t_command *cmd);

#endif
