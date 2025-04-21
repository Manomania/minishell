/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmdas_utils7.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 19:18:09 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/21 19:21:00 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Setup child process redirection for pipeline
 *
 * @param ctx Context
 * @param current Current command structure
 * @param prev_pipe Previous pipe read end
 * @param next_write Next pipe write end
 * @return 0 on success, -1 on error
 */
int	setup_child_redirections_bruh(t_ctx *ctx, t_command *current, int prev_pipe,
		int next_write)
{
	int	result;

	result = setup_child_pipeline_redirections(current, prev_pipe, next_write);
	if (result != 0)
		return (-1);
	if (setup_heredocs(ctx, current) != 0)
		return (-1);
	return (0);
}

/**
 * Execute command in child process and handle exit
 *
 * @param ctx Context information
 * @param current Current command structure
 */
void	handle_child_process(t_ctx *ctx, t_command *current)
{
	reset_signals();
	if (!current->args || !current->args[0])
	{
		cleanup_child_process(ctx);
		exit(EXIT_SUCCESS);
	}
	execute_command(ctx, current);
	cleanup_child_process(ctx);
	exit(127);
}
