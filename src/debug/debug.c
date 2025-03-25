/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 19:20:06 by maximart          #+#    #+#             */
/*   Updated: 2025/03/24 14:00:43 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "debug.h"

int		g_debug_level = DEBUG_NONE;

/**
 * @brief Initialize debug system with specified verbosity level
 *
 * @param level Debug verbosity level
 */
void	debug_init(int level)
{
	g_debug_level = level;
}

/**
 * @brief Log a debug message if current level is high enough
 *
 * @param level Required debug level to show this message
 * @param module Module name for the log
 * @param msg Log message to display
 */
void	debug_log(int level, const char *module, const char *msg)
{
	if (g_debug_level >= level)
	{
		ft_putstr_fd((char *)CYAN, STDERR_FILENO);
		ft_putstr_fd((char *)"[DEBUG] ", STDERR_FILENO);
		if (module)
		{
			ft_putstr_fd((char *)module, STDERR_FILENO);
			ft_putstr_fd((char *)": ", STDERR_FILENO);
		}
		ft_putstr_fd((char *)msg, STDERR_FILENO);
		ft_putstr_fd((char *)RESET, STDERR_FILENO);
		ft_putstr_fd((char *)"\n", STDERR_FILENO);
	}
}

/**
 * @brief Print a single token for debugging
 *
 * @param level Required debug level to show this
 * @param token Token to print
 */
void	debug_print_token(int level, t_token *token)
{
	char	buffer[512];

	if (g_debug_level < level || !token)
		return ;
	ft_strlcpy(buffer, "Token: Type=", sizeof(buffer));
	ft_strlcat(buffer, get_token_type_str(token->type), sizeof(buffer));
	ft_strlcat(buffer, ", Value=", sizeof(buffer));
	if (token->value)
		ft_strlcat(buffer, token->value, sizeof(buffer));
	else
		ft_strlcat(buffer, "NULL", sizeof(buffer));
	debug_log(level, "lexer", buffer);
}

/**
 * @brief Print all tokens in a list for debugging
 *
 * @param level Required debug level to show this
 * @param tokens Tokens list to print
 */
void	debug_print_tokens(int level, t_token *tokens)
{
	t_token	*current;
	int		count;

	if (g_debug_level < level)
		return ;
	debug_log(level, "lexer", "Token list:");
	current = tokens;
	count = 0;
	while (current)
	{
		debug_print_token(level, current);
		current = current->next;
		count++;
	}
}

/**
 * @brief Print a command structure for debugging
 *
 * @param level Required debug level to show this
 * @param cmd Command to print
 */
void	debug_print_command(int level, t_command *cmd)
{
	char			buffer[512];
	t_redirection	*redir;
	char			*count_str;
	int				i;

	if (g_debug_level < level || !cmd)
		return ;
	debug_log(level, "parser", "Command:");
	if (cmd->args && cmd->args[0])
	{
		ft_strlcpy(buffer, "Command name: ", sizeof(buffer));
		ft_strlcat(buffer, cmd->args[0], sizeof(buffer));
		debug_log(level, "parser", buffer);
	}
	i = 1;
	while (cmd->args && i <= cmd->arg_count)
	{
		ft_strlcpy(buffer, "Arg[", sizeof(buffer));
		count_str = ft_itoa(i - 1);
		ft_strlcat(buffer, count_str, sizeof(buffer));
		free(count_str);
		ft_strlcat(buffer, "]: ", sizeof(buffer));
		ft_strlcat(buffer, cmd->args[i], sizeof(buffer));
		debug_log(level, "parser", buffer);
		i++;
	}
	redir = cmd->redirection;
	while (redir)
	{
		ft_strlcpy(buffer, "Redirection: type=", sizeof(buffer));
		ft_strlcat(buffer, get_token_type_str(redir->type), sizeof(buffer));
		ft_strlcat(buffer, ", file=", sizeof(buffer));
		ft_strlcat(buffer, redir->filename, sizeof(buffer));
		debug_log(level, "parser", buffer);
		redir = redir->next;
	}
}

/**
 * @brief Print all commands in a pipeline for debugging
 *
 * @param level Required debug level to show this
 * @param cmd First command in the pipeline
 */
void	debug_print_commands(int level, t_command *cmd)
{
	t_command	*current;
	int			count;
	char		buffer[64];
	char		*count_str;

	if (g_debug_level < level)
		return ;
	debug_log(level, "parser", "Command pipeline:");
	current = cmd;
	count = 0;
	while (current)
	{
		count_str = ft_itoa(count);
		ft_strlcpy(buffer, "Command #", sizeof(buffer));
		ft_strlcat(buffer, count_str, sizeof(buffer));
		free(count_str);
		debug_log(level, "parser", buffer);
		debug_print_command(level, current);
		if (current->next)
		{
			ft_strlcpy(buffer, "Operator: ", sizeof(buffer));
			ft_strlcat(buffer, get_token_type_str(current->operator),
				sizeof(buffer));
			debug_log(level, "parser", buffer);
		}
		current = current->next;
		count++;
	}
}
