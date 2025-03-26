/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_utils3.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 11:40:31 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/26 12:48:12 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "debug.h"
#include "minishell.h"

void	print_redirections(t_redirection *redir)
{
	t_redirection	*current;
	int				redir_count;

	redir_count = 0;
	current = redir;
	if (!current)
	{
		ft_printf(YELLOW "  No redirections\n" RESET);
		return ;
	}
	ft_printf(YELLOW "  Redirections:\n" RESET);
	while (current)
	{
		ft_printf(GREEN "    Redirection %d: " RESET, redir_count++);
		ft_printf("Type: " YELLOW);
		print_redirection_type(current->type);
		ft_printf(RESET);
		ft_printf(" | Filename: " GREEN "%s" RESET, current->filename);
		ft_printf("\n");
		current = current->next;
	}
}

/**
 * @brief Extracts the display value by removing quote characters
 *
 * This function creates a copy of the input string with all single and
 * double quote characters removed, while respecting quote pairing.
 * Quotes inside opposite quotes are preserved.
 *
 * @param str The input string to process
 * @return A newly allocated string with quotes removed, or error message
 * if allocation fails
 *         Returns "NULL" if input is NULL
 *         Returns "[malloc error]" if memory allocation fails
 */
char	*get_display_value(char *str)
{
	int		i;
	int		j;
	char	*result;
	int		quotes[2];

	if (!str)
		return (ft_strdup("NULL"));
	result = malloc(ft_strlen(str) + 1);
	if (!result)
		return (ft_strdup("[malloc error]"));
	i = -1;
	j = 0;
	quotes[0] = 0;
	quotes[1] = 0;
	while (str[++i])
	{
		if (str[i] == '\'' && !quotes[1])
			quotes[0] = !quotes[0];
		else if (str[i] == '"' && !quotes[0])
			quotes[1] = !quotes[1];
		else
			result[j++] = str[i];
	}
	result[j] = '\0';
	return (result);
}

void	print_command_args(t_command *cmd)
{
	int		i;
	char	*display_value;

	i = 0;
	if (!cmd->args || cmd->arg_count == 0)
	{
		ft_printf(YELLOW "  No arguments\n" RESET);
		return ;
	}
	ft_printf(YELLOW "  Arguments:\n" RESET);
	while (i < cmd->arg_count)
	{
		display_value = get_display_value(cmd->args[i]);
		ft_printf(GREEN "    Arg %d: " RESET "%s\n", i, display_value);
		free(display_value);
		i++;
	}
}

void	print_operator_type(t_token_type op_type)
{
	if (op_type == TOK_PIPE)
		ft_printf(BLUE "  Piped to next command (|)\n" RESET);
}

void	print_commands(t_command *cmd)
{
	t_command	*current;
	int			cmd_count;

	cmd_count = 0;
	current = cmd;
	ft_printf(YELLOW "\n===== COMMANDS =====\n" RESET);
	if (!current)
	{
		ft_printf(RED "No commands to display\n" RESET);
		ft_printf(YELLOW "===================\n\n" RESET);
		return ;
	}
	while (current)
	{
		ft_printf(GREEN "Command %d:\n" RESET, cmd_count++);
		print_command_args(current);
		print_redirections(current->redirection);
		if (current->next)
			print_operator_type(current->operator);
		current = current->next;
	}
	ft_printf(YELLOW "===================\n\n" RESET);
}
