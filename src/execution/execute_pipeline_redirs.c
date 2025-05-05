/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipeline_redirs.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 15:44:33 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/05 15:45:29 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Handle file descriptor duplications
 *
 * Duplicates input and output file descriptors
 *
 * @param fds File descriptors to duplicate
 */
void	handle_fd_redirection(t_fds fds)
{
	if (fds.in != -1)
	{
		if (dup2(fds.in, STDIN_FILENO) == -1)
			exit(1);
		close(fds.in);
	}
	if (fds.out != -1)
	{
		if (dup2(fds.out, STDOUT_FILENO) == -1)
			exit(1);
		close(fds.out);
	}
}
