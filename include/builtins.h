/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 09:51:56 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/21 10:20:36 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUILTINS_H
# define BUILTINS_H

# include "minishell.h"

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
pid_t	execute_pipeline_command(t_ctx *ctx, t_command *cmd, int input_fd,
			int output_fd);

#endif
