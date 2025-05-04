/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 16:44:50 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/04 19:41:16 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "execute.h"

/**
 * @brief Checks if delimiter is quoted
 *
 * @param delimiter Heredoc delimiter
 * @return t_bool true if quoted, false otherwise
 */
static t_bool	is_delimiter_quoted(char *delimiter)
{
	int	len;

	len = ft_strlen(delimiter);
	if (len >= 2 && ((delimiter[0] == '\'' && delimiter[len - 1] == '\'')
			|| (delimiter[0] == '"' && delimiter[len - 1] == '"')))
		return (true);
	return (false);
}

/**
 * @brief Removes quotes from delimiter if present
 *
 * @param delimiter Original delimiter
 * @return char* Unquoted delimiter or NULL on error
 */
static char	*unquote_delimiter(char *delimiter)
{
	int	len;

	len = ft_strlen(delimiter);
	if (is_delimiter_quoted(delimiter))
		return (ft_substr(delimiter, 1, len - 2));
	return (ft_strdup(delimiter));
}

/**
 * @brief Runs the heredoc input reader process
 *
 * @param pipes Open pipe file descriptors
 * @param delimiter Heredoc delimiter
 * @param quoted Whether delimiter is quoted
 * @param ctx Context for variable expansion
 */
static void	run_heredoc_reader(int *pipes, char *delim, t_bool quoted,
		t_ctx *ctx)
{
	char	*line;
	char	*processed;
	char	*clean_delim;

	close(pipes[0]);
	setup_heredoc_signals();
	clean_delim = unquote_delimiter(delim);
	if (!clean_delim)
		exit(1);
	while (1)
	{
		line = readline("> ");
		if (!line || ft_strncmp(line, clean_delim, ft_strlen(clean_delim)
				+ 1) == 0)
		{
			if (!line)
				ft_printf_fd(STDERR_FILENO,
					"minishell: warning: here-document delimited by end-of-file (wanted `%s')\n",
					clean_delim);
			free(line);
			break ;
		}
		if (quoted)
			processed = ft_strdup(line);
		else
			processed = handle_quotes_and_vars(ctx, line);
		if (processed)
		{
			write(pipes[1], processed, ft_strlen(processed));
			write(pipes[1], "\n", 1);
			free(processed);
		}
		free(line);
	}
	free(clean_delim);
	ctx_clear(ctx);
	close(pipes[1]);
	exit(0);
}

/**
 * @brief Waits for heredoc process and handles result
 *
 * @param pid Child process ID
 * @param pipes Pipe file descriptors
 * @param ctx Context for error handling
 * @return int Pipe read fd or -1 on error
 */
static int	wait_heredoc_process(pid_t pid, int *pipes, t_ctx *ctx)
{
	int	status;

	close(pipes[1]);
	waitpid(pid, &status, 0);
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
	{
		close(pipes[0]);
		ctx->exit_status = 130;
		return (-1);
	}
	if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
	{
		close(pipes[0]);
		return (-1);
	}
	return (pipes[0]);
}

/**
 * @brief Handles a single heredoc redirection
 *
 * @param ctx Context containing environment
 * @param redir Redirection structure to process
 * @return t_bool true on success, false on error
 */
static t_bool	handle_heredoc(t_ctx *ctx, t_redirection *redir)
{
	t_bool	quoted;
	pid_t	pid;
	int		pipes[2];
	int		fd;

	quoted = is_delimiter_quoted(redir->filename);
	if (pipe(pipes) == -1)
		return (false);
	pid = fork();
	if (pid == 0)
		run_heredoc_reader(pipes, redir->filename, quoted, ctx);
	else if (pid > 0)
	{
		fd = wait_heredoc_process(pid, pipes, ctx);
		if (fd == -1)
			return (false);
		redir->fd = fd;
		return (true);
	}
	return (false);
}

/**
 * @brief Processes all heredocs in a command list
 *
 * @param ctx Context containing environment
 * @param cmd Command list to process
 * @return t_bool true on success, false on error
 */
t_bool	process_heredocs(t_ctx *ctx, t_command *cmd)
{
	t_command		*current;
	t_redirection	*redir;

	current = cmd;
	while (current)
	{
		redir = current->redirection;
		while (redir)
		{
			if (redir->type == TOK_HERE_DOC_FROM)
			{
				if (!handle_heredoc(ctx, redir))
					return (false);
			}
			redir = redir->next;
		}
		current = current->next;
	}
	return (true);
}
