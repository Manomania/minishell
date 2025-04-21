/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 09:51:56 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/15 15:57:08 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUILTINS_H
# define BUILTINS_H

# include "minishell.h"

// *************************************************************************** #
//                            Function Prototypes                              #
// *************************************************************************** #

// builtin_cd.c
char	*get_target_directory(t_ctx *ctx, t_command *cmd);
int		builtin_cd(t_ctx *ctx, t_command *cmd);

// builtin_cd_utils.c
char	*get_old_pwd(t_ctx *ctx);
char	*get_home_dir(t_ctx *ctx);
t_env	*update_pwd_variable(t_env *env_node, char *new_pwd);
void	update_oldpwd_variable(t_env *old_pwd_node, char *old_pwd);
char	*handle_dot_directory(t_ctx *ctx);

// builtin_echo.c
int		builtin_echo(t_ctx *ctx, t_command *cmd);

// builtin_env.c
int		builtin_env(t_ctx *ctx, t_command *cmd);

// builtin_exit.c
int		builtin_exit(t_ctx *ctx, t_command *cmd);

// builtin_export.c
t_bool	validate_env_key(char *key);
char	*get_env_value_from_export(char *arg);
int		builtin_export(t_ctx *ctx, t_command *cmd);

// builtin_export_utils.c
t_bool	is_valid_env_char(char c);
char	*get_env_key_from_export(char *arg);
void	print_export_env(t_ctx *ctx);
t_bool	update_env_var(t_env **env_list, char *key, char *value,
			t_bool has_equals);

// builtin_export_utils2.c
int		extract_export_data(char *arg, char **key_ptr, char **value_ptr,
			t_bool *has_equals_ptr);
int		update_or_add_env_var(t_ctx *ctx, char *key, char *value,
			t_bool has_equals);

// builtin_pipeline.c
t_bool	is_builtin_command(char *cmd_name);
pid_t	execute_pipeline_command(t_ctx *ctx, t_command *cmd, int *input_fd,
			int *output_fd);

// builtin_pipeline_utils.c
void	restore_pipeline_fds(int saved_in, int saved_out);

// builtin_pipeline_utils2.c
int		setup_builtin_pipeline_fds(int *fds, int saved_fds[2]);
int		execute_builtin_command(t_ctx *ctx, t_command *cmd);
int		run_builtin_with_redir(t_ctx *ctx, t_command *cmd);
void	restore_all_fds(int saved_fds[2]);
int		prepare_and_run_builtin(t_ctx *ctx, t_command *cmd, int *fds);

// builtin_pwd.c
int		builtin_pwd(t_ctx *ctx, t_command *cmd);

// builtin_unset.c
t_bool	remove_env_var(t_env **env_list, char *key);
int		builtin_unset(t_ctx *ctx, t_command *cmd);

// builtins_redirect.c
int		open_redirection_file(t_redirection *redir);
int		builtin_setup_redirections(t_command *cmd, int saved_fds[2]);
void	builtin_restore_redirections(int saved_fds[2]);

// builtin_redirect_utils.c
t_bool	is_here_doc_input(t_redirection *redir);
int		apply_input_redirection(int fd);
int		apply_output_redirection(int fd);
int		apply_redirection(t_redirection *redir);

// builtin_try.c
t_bool	builtins_try(t_ctx *ctx, t_command *cmd);

// builtin_try_utils.c
int		is_builtin(char *cmd_name, char *builtin_name);
int		setup_builtin_redirections(t_command *cmd, int *saved_fds);
void	run_builtin_command(t_ctx *ctx, t_command *cmd, int *exit_status);

// builtin_try_utils2.c
t_bool	execute_builtin(t_ctx *ctx, t_command *cmd, int *exit_status);

#endif
