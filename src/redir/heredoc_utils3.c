/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils3.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 12:55:14 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/23 12:57:43 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "minishell.h"

/**
 * @brief Processes heredoc content and writes to pipe
 *
 * @param pipe_fd Write end of pipe
 * @param delimiter String marking end of heredoc
 * @param ctx Shell context
 * @return 0 on success, non-zero on error
 */
int	process_heredoc_content(int pipe_fd, char *delimiter, t_ctx *ctx)
{
	char	*line;
	char	*expanded_line;
	int		status;

	while (1)
	{
		status = read_heredoc_line(delimiter, &line);
		if (status != 0)
		{
			if (status < 0)
				return (1);
			return (0);
		}
		expanded_line = expand_variables_in_line(ctx, line);
		free(line);
		if (!expanded_line)
			return (-1);
		write(pipe_fd, expanded_line, ft_strlen(expanded_line));
		write(pipe_fd, "\n", 1);
		free(expanded_line);
	}
	return (0);
}
