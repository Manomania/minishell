/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 13:59:38 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/23 12:58:56 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "error.h"
#include "minishell.h"

static void	display_heredoc_eof_warning(void)
{
	char	*warning_part1;
	char	*warning_part2;
	char	*warning_part3;

	warning_part1 = "minishell: warning: heredoc: ";
	warning_part2 = "here-document delimited by end-of-file ";
	warning_part3 = "(`eof')\n";
	ft_printf_fd(STDERR_FILENO, YELLOW "%s%s%s" RESET, warning_part1,
		warning_part2, warning_part3);
}

int	read_heredoc_line(char *delimiter, char **line)
{
	int	delimiter_len;

	if (g_signal_status == 130)
		return (-1);
	delimiter_len = ft_strlen(delimiter);
	rl_catch_signals = 0;
	rl_catch_sigwinch = 0;
	*line = readline("> ");
	if (!(*line) || g_signal_status == 130)
	{
		if (*line && g_signal_status == 130)
			free(*line);
		else if (!(*line))
			display_heredoc_eof_warning();
		*line = NULL;
		return (-1);
	}
	if (ft_strncmp(*line, delimiter, delimiter_len + 1) == 0)
	{
		free(*line);
		return (1);
	}
	return (0);
}
