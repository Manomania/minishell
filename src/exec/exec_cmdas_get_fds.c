/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmdas_get_fds.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/30 18:01:10 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/11 10:31:48 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Gets the input file descriptor for a command
 *
 * @param ctx Context
 * @param fd_pipes Pipes
 * @param cmd_index Index of the command
 * @return int File descriptor
 */
int	get_fd_in(t_ctx *ctx, t_list *fd_pipes, int cmd_index)
{
	if (cmd_index == 0)
		return (ctx->fd_file_in);
	if (!fd_pipes || !fd_pipes->content)
		return (-1);
	return (((int *)fd_pipes->content)[0]);
}

/**
 * @brief Gets the output file descriptor for a command
 *
 * @param ctx Context
 * @param fd_pipes Pipes
 * @param cmd_index Index of the command
 * @param cmd_count Number of commands
 * @return int File descriptor
 */
int	get_fd_out(t_ctx *ctx, t_list *fd_pipes, int cmd_index, int cmd_count)
{
	if (cmd_index == cmd_count - 1)
		return (ctx->fd_file_out);
	if (!fd_pipes || !fd_pipes->content)
		return (-1);
	return (((int *)fd_pipes->content)[1]);
}
