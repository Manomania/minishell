/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 13:22:49 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/28 13:23:10 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"

/**
 * @brief Handles errors for file operations in redirections.
 *
 * @param filename The filename that caused the error
 * @param last_input_fd Previous input file descriptor to close if needed
 * @param last_output_fd Previous output file descriptor to close if needed
 * @return -1 to indicate error
 */
int	handle_redirection_error(char *filename, int last_input_fd,
		int last_output_fd)
{
	if (errno == EACCES)
		error(filename, "redir", ERR_NO_PERMS);
	else
		error(filename, "redir", ERR_NO_FILE);
	if (last_input_fd != -1)
		close(last_input_fd);
	if (last_output_fd != -1)
		close(last_output_fd);
	return (-1);
}
