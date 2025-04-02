/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 09:51:56 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/30 17:53:44 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUILTINS_H
# define BUILTINS_H

# include "minishell.h"

/**
 * @brief Determines the target directory for cd command
 *
 * @param ctx Context for shell environment
 * @param cmd Command containing arguments
 * @return char* Target directory path (must be freed by caller)
 */
char	*get_target_directory(t_ctx *ctx, t_command *cmd);

/**
 * @brief Updates PWD environment variable
 *
 * @param env_node Current environment node
 * @param new_pwd New PWD value
 * @return t_env* Pointer to OLDPWD node if found, NULL otherwise
 */
t_env	*update_pwd_variable(t_env *env_node, char *new_pwd);

/**
 * @brief Updates OLDPWD environment variable
 *
 * @param old_pwd_node OLDPWD environment node
 * @param old_pwd Old PWD value
 * @return void
 */
void	update_oldpwd_variable(t_env *old_pwd_node, char *old_pwd);

/**
 * @brief Restores original file descriptors
 *
 * @param saved_in Saved stdin file descriptor
 * @param saved_out Saved stdout file descriptor
 */
void	restore_pipeline_fds(int saved_in, int saved_out);

/**
 * @brief Opens a file for redirection
 *
 * @param redir Redirection information
 * @return int File descriptor or -1 on error
 */
int		open_redirection_file(t_redirection *redir);

/**
 * @brief Checks if a character is valid for env variable name
 *
 * @param c Character to check
 * @return t_bool true if valid, false otherwise
 */
t_bool	is_valid_env_char(char c);

/**
 * @brief Gets the key part of an environment variable
 *
 * @param arg Argument string (key=value)
 * @return char* Key part (caller must free)
 */
char	*get_env_key_from_export(char *arg);

/**
 * @brief Removes an environment variable from the list
 *
 * @param env_list Pointer to environment list
 * @param key Key to remove
 * @return t_bool true if removed, false if not found
 */
t_bool	remove_env_var(t_env **env_list, char *key);

/**
 * @brief Prints all environment variables in export format
 *
 * @param ctx Context for shell environment
 * @return void
 */
void	print_export_env(t_ctx *ctx);

/**
 * @brief Updates an existing environment variable
 *
 * @param env_list Pointer to environment list
 * @param key Key to update
 * @param value New value
 * @param has_equals Whether the variable has an equals sign
 * @return t_bool true if updated, false if not found
 */
t_bool	update_env_var(t_env **env_list, char *key, char *value,
			t_bool has_equals);

/**
 * @brief Checks if redirection is a here-doc input
 *
 * @param redir Redirection to check
 * @return t_bool true if here-doc input, false otherwise
 */
t_bool	is_here_doc_input(t_redirection *redir);

/**
 * @brief Applies input redirection
 *
 * @param fd File descriptor to redirect from
 * @return int 0 on success, -1 on error
 */
int		apply_input_redirection(int fd);

/**
 * @brief Applies output redirection
 *
 * @param fd File descriptor to redirect to
 * @return int 0 on success, -1 on error
 */
int		apply_output_redirection(int fd);

/**
 * @brief Applies a single redirection
 *
 * @param redir Redirection to apply
 * @return int 0 on success, -1 on error
 */
int		apply_redirection(t_redirection *redir);

/**
 * @brief Compares a command name with a potential built-in
 *
 * @param cmd_name Command name to check
 * @param builtin_name Built-in name to compare against
 * @return int 0 if they match, non-zero otherwise
 */
int		is_builtin(char *cmd_name, char *builtin_name);

/**
 * @brief Sets up redirections for builtin command execution
 *
 * @param cmd Command to set up redirections for
 * @param saved_fds Array of saved file descriptors
 * @return int 0 on success, -1 on failure
 */
int		setup_builtin_redirections(t_command *cmd, int *saved_fds);

/**
 * @brief Executes the appropriate builtin command
 *
 * @param ctx Context for shell environment
 * @param cmd Command to execute
 * @param exit_status Pointer to store exit status
 */
void	run_builtin_command(t_ctx *ctx, t_command *cmd, int *exit_status);

/**
 * @brief Checks if a command is a built-in and executes it
 *
 * @param ctx Context for shell environment
 * @param cmd Command to check for builtin
 * @return t_bool true if builtin was executed, false otherwise
 */
t_bool	builtins_try(t_ctx *ctx, t_command *cmd);

/**
 * @brief Executes the echo built-in command
 *
 * @param ctx Context for shell environment
 * @param cmd Command containing arguments
 * @return int Exit status (0 for success, non-zero for error)
 */
int		builtin_echo(t_ctx *ctx, t_command *cmd);

/**
 * @brief Executes the cd built-in command
 *
 * @param ctx Context for shell environment
 * @param cmd Command containing arguments
 * @return int Exit status (0 for success, non-zero for error)
 */
int		builtin_cd(t_ctx *ctx, t_command *cmd);

/**
 * @brief Executes the pwd built-in command
 *
 * @param ctx Context for shell environment
 * @param cmd Command containing arguments
 * @return int Exit status (0 for success, non-zero for error)
 */
int		builtin_pwd(t_ctx *ctx, t_command *cmd);

/**
 * @brief Executes the export built-in command
 *
 * @param ctx Context for shell environment
 * @param cmd Command containing arguments
 * @return int Exit status (0 for success, non-zero for error)
 */
int		builtin_export(t_ctx *ctx, t_command *cmd);

/**
 * @brief Executes the unset built-in command
 *
 * @param ctx Context for shell environment
 * @param cmd Command containing arguments
 * @return int Exit status (0 for success, non-zero for error)
 */
int		builtin_unset(t_ctx *ctx, t_command *cmd);

/**
 * @brief Executes the env built-in command
 *
 * @param ctx Context for shell environment
 * @param cmd Command containing arguments
 * @return int Exit status (0 for success, non-zero for error)
 */
int		builtin_env(t_ctx *ctx, t_command *cmd);

/**
 * @brief Executes the exit built-in command
 *
 * @param ctx Context for shell environment
 * @param cmd Command containing arguments
 * @return int Exit status (0 for success, non-zero for error)
 */
int		builtin_exit(t_ctx *ctx, t_command *cmd);

/**
 * @brief Handles redirections for built-in commands
 *
 * @param cmd Command containing redirections
 * @param saved_fds Array to save original file descriptors
 * @return int 0 on success, -1 on error
 */
int		builtin_setup_redirections(t_command *cmd, int saved_fds[2]);

/**
 * @brief Restores original file descriptors after built-in execution
 *
 * @param saved_fds Array containing saved file descriptors
 * @return void
 */
void	builtin_restore_redirections(int saved_fds[2]);

/**
 * @brief Checks if a command is a built-in
 *
 * @param cmd_name Command name to check
 * @return t_bool true if built-in, false otherwise
 */
t_bool	is_builtin_command(char *cmd_name);

/**
 * @brief Executes a command in a pipeline context,
 * handling built-ins specially
 *
 * @param ctx Context for shell environment
 * @param cmd Command to execute
 * @param input_fd Input file descriptor
 * @param output_fd Output file descriptor
 * @return pid_t Process ID or -2 for built-in execution
 */
pid_t	execute_pipeline_command(t_ctx *ctx, t_command *cmd, int *input_fd,
			int *output_fd);

/**
 * @brief Handles cd . when current directory doesn't exist
 *
 * @param ctx Context for shell environment
 * @return char* Directory path or NULL if not found
 */
char	*handle_dot_directory(t_ctx *ctx);

/**
 * @brief Gets the old PWD directory from environment
 *
 * @param ctx Context for shell environment
 * @return char* Old PWD or NULL if not found
 */
char	*get_old_pwd(t_ctx *ctx);

/**
 * @brief Gets the home directory from environment
 *
 * @param ctx Context for shell environment
 * @return char* Home directory or NULL if not found
 */
char	*get_home_dir(t_ctx *ctx);

#endif
