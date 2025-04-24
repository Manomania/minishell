/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 15:30:10 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/23 12:57:15 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"

static void	restore_shell_state(void)
{
	setup_signals();
	rl_event_hook = NULL;
	rl_clear_signals();
	rl_on_new_line();
	rl_replace_line("", 0);
}

int	create_heredoc(t_ctx *ctx, char *delimiter)
{
	int		pipe_fds[2];
	int		read_fd;
	int		result;

	if (pipe(pipe_fds) == -1)
		return (error(NULL, "heredoc", ERR_PIPE));
	setup_heredoc_signals();
	result = read_heredoc_content(pipe_fds, delimiter, ctx);
	close(pipe_fds[1]);
	if (result == -1 || is_heredoc_interrupted())
	{
		close(pipe_fds[0]);
		if (is_heredoc_interrupted())
		{
			ctx->exit_status = 130;
			g_signal_status = 0;
		}
		restore_shell_state();
		return (-1);
	}
	read_fd = pipe_fds[0];
	restore_shell_state();
	return (read_fd);
}

int	setup_heredocs(t_ctx *ctx, t_command *cmd)
{
	t_redirection	*redir;
	int				dup_result;

	(void)ctx;
	redir = cmd->redirection;
	while (redir)
	{
		if (redir->type == TOK_HERE_DOC_FROM && redir->fd >= 0)
		{
			dup_result = dup2(redir->fd, STDIN_FILENO);
			close(redir->fd);
			redir->fd = -1;
			if (dup_result == -1)
			{
				perror("dup2");
				return (-1);
			}
		}
		redir = redir->next;
	}
	return (0);
}
