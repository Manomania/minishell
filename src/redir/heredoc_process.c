/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_process.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maximart <maximart@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 12:10:14 by maximart          #+#    #+#             */
/*   Updated: 2025/04/24 12:10:16 by maximart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"

static int	process_heredoc_line(int fd, char *line, t_ctx *ctx)
{
	char	*expanded_line;

	expanded_line = expand_variables_in_line(ctx, line);
	free(line);
	if (!expanded_line)
		return (-1);
	write(fd, expanded_line, ft_strlen(expanded_line));
	write(fd, "\n", 1);
	free(expanded_line);
	return (0);
}

int	read_heredoc_content(int *pipe_fds, char *delimiter, t_ctx *ctx)
{
	char	*line;
	int		status;

	g_signal_status = 0;
	rl_event_hook = interrupt_check_hook;
	while (1)
	{
		if (is_heredoc_interrupted())
			return (-1);
		status = read_heredoc_line(delimiter, &line);
		if (is_heredoc_interrupted())
		{
			if (line)
				free(line);
			return (-1);
		}
		if (status != 0)
		{
			if (status < 0)
				return (-1);
			return (0);
		}
		if (process_heredoc_line(pipe_fds[1], line, ctx) == -1)
			return (-1);
	}
}

t_bool	has_heredoc_redirection(t_command *cmd)
{
	t_redirection	*redir;

	if (!cmd || !cmd->redirection)
		return (false);
	redir = cmd->redirection;
	while (redir)
	{
		if (redir->type == TOK_HERE_DOC_FROM)
			return (true);
		redir = redir->next;
	}
	return (false);
}
