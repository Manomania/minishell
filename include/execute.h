/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 16:43:21 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/02 16:56:58 by elagouch         ###   ########.fr       */
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
t_bool	process_heredocs(t_ctx *ctx, t_command *cmd);

// redirection.c
t_bool	apply_redirections(t_command *cmd);
t_bool	save_original_fds(int *stdin_fd, int *stdout_fd);
void	restore_original_fds(int stdin_fd, int stdout_fd);

// free_command.c
void	free_command(t_command *cmd);
void	free_all_commands(t_command *cmd);

#endif
