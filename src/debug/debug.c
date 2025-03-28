/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 19:20:06 by maximart          #+#    #+#             */
/*   Updated: 2025/03/27 17:46:48 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "debug.h"

/**
 * @brief Log a debug message if current level is high enough
 *
 * @param module Module name for the log
 * @param msg Log message to display
 */
void	debug_log(const char *module, const char *msg)
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

/**
 * @brief Print a single token for debugging
 *
 * @param level Required debug level to show this
 * @param token Token to print
 */
void	debug_print_token(t_token *token)
{
	char	buffer[512];

	ft_strlcpy(buffer, "Token: Type=", sizeof(buffer));
	ft_strlcat(buffer, get_token_type_str(token->type), sizeof(buffer));
	ft_strlcat(buffer, ", Value=", sizeof(buffer));
	if (token->value)
		ft_strlcat(buffer, token->value, sizeof(buffer));
	else
		ft_strlcat(buffer, "NULL", sizeof(buffer));
	debug_log("lexer", buffer);
}

/**
 * @brief Print all tokens in a list for debugging
 *
 * @param tokens Tokens list to print
 */
void	debug_print_tokens(t_token *tokens)
{
	t_token	*current;
	int		count;

	debug_log("lexer", "Token list:");
	current = tokens;
	count = 0;
	while (current)
	{
		debug_print_token(current);
		current = current->next;
		count++;
	}
}

/**
 * @brief Print a command structure for debugging
 *
 * @param cmd Command to print
 */
void	debug_print_command(t_command *cmd)
{
	char			buffer[512];
	t_redirection	*redir;
	char			*count_str;
	int				i;

	debug_log("parser", "Command:");
	if (cmd->args && cmd->args[0])
	{
		ft_strlcpy(buffer, "Command name: ", sizeof(buffer));
		ft_strlcat(buffer, cmd->args[0], sizeof(buffer));
		debug_log("parser", buffer);
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
		debug_log("parser", buffer);
		i++;
	}
	redir = cmd->redirection;
	while (redir)
	{
		ft_strlcpy(buffer, "Redirection: type=", sizeof(buffer));
		ft_strlcat(buffer, get_token_type_str(redir->type), sizeof(buffer));
		ft_strlcat(buffer, ", file=", sizeof(buffer));
		ft_strlcat(buffer, redir->filename, sizeof(buffer));
		debug_log("parser", buffer);
		redir = redir->next;
	}
}

/**
 * @brief Print all commands in a pipeline for debugging
 *
 * @param cmd First command in the pipeline
 */
void	debug_print_commands(t_command *cmd)
{
	t_command	*current;
	int			count;
	char		buffer[64];
	char		*count_str;

	debug_log("parser", "Command pipeline:");
	current = cmd;
	count = 0;
	while (current)
	{
		count_str = ft_itoa(count);
		ft_strlcpy(buffer, "Command #", sizeof(buffer));
		ft_strlcat(buffer, count_str, sizeof(buffer));
		free(count_str);
		debug_log("parser", buffer);
		debug_print_command(current);
		if (current->next)
		{
			ft_strlcpy(buffer, "Operator: ", sizeof(buffer));
			ft_strlcat(buffer, get_token_type_str(current->operator),
				sizeof(buffer));
			debug_log("parser", buffer);
		}
		current = current->next;
		count++;
	}
}
