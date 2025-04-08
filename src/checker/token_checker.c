/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_checker.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maximart <maximart@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 08:32:58 by maximart          #+#    #+#             */
/*   Updated: 2025/03/26 08:33:25 by maximart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "minishell.h"
# include "error.h"


/**
 * @brief Checks for invalid redirection sequences
 *
 * @param tokens Token list to validate
 * @return t_bool false if invalid redirection found, true otherwise
 */
static t_bool	check_invalid_tokens(t_token *tokens)
{
	t_token	*current;

	current = tokens;
	while (current && current->next)
	{
		if (current->type == TOK_ESP)
		{
			ft_putendl_fd("\001" RED "\002syntax error near unexpected token '&'"RESET, STDERR_FILENO);
			return (false);
		}
		if (current->type == TOK_OR)
		{
			ft_putendl_fd("\001" RED "\002syntax error near unexpected token '||'"RESET, STDERR_FILENO);
			return (false);
		}
		if (current->type == TOK_AND)
		{
			ft_putendl_fd("\001" RED "\002syntax error near unexpected token '&&'"RESET, STDERR_FILENO);
			return (false);
		}
		if (current->type == TOK_REDIR_FROM && current->next->type == TOK_REDIR_TO)
		{
			ft_putendl_fd("\001" RED "\002syntax error near unexpected token 'newline'"RESET, STDERR_FILENO);
			return (false);
		}
		if (token_is_redirection(current->type) && current->next->type == TOK_REDIR_TO)
		{
			ft_putendl_fd("\001" RED "\002syntax error near unexpected token '>'"RESET, STDERR_FILENO);
			return (false);
		}
		if (token_is_redirection(current->type) && current->next->type == TOK_REDIR_FROM)
		{
			ft_putendl_fd("\001" RED "\002syntax error near unexpected token '<'"RESET, STDERR_FILENO);
			return (false);
		}
		if ((current->type == TOK_PIPE && token_is_redirection(current->next->type))
			|| (token_is_redirection(current->type) && current->next->type == TOK_PIPE))
		{
			ft_putendl_fd("\001" RED "\002syntax error near unexpected token '|'"RESET, STDERR_FILENO);
			return (false);
		}
		if (current->type == TOK_PIPE && (current->next->type == TOK_PIPE
			|| current->next->type == TOK_EOF))
		{
			ft_putendl_fd("\001" RED "\002syntax error near unexpected token '|'"RESET, STDERR_FILENO);
			return (false);
		}
		if (current->type == TOK_REDIR_FROM && current->next->type == TOK_REDIR_FROM)
		{
			ft_putendl_fd("\001" RED "\002syntax error near unexpected token '<'"RESET, STDERR_FILENO);
			return (false);
		}
		if (current->type == TOK_REDIR_TO && current->next->type == TOK_REDIR_TO)
		{
			ft_putendl_fd("\001" RED "\002syntax error near unexpected token '>'"RESET, STDERR_FILENO);
			return (false);
		}
		if (current->type == TOK_HERE_DOC_TO && current->next->type == TOK_HERE_DOC_TO)
		{
			ft_putendl_fd("\001" RED "\002syntax error near unexpected token '>>'"RESET, STDERR_FILENO);
			return (false);
		}
		if (current->type == TOK_HERE_DOC_FROM && current->next->type == TOK_HERE_DOC_FROM)
		{
			ft_putendl_fd("\001" RED "\002syntax error near unexpected token '<<'"RESET, STDERR_FILENO);
			return (false);
		}
		if (token_is_redirection(current->type)
			&& (token_is_redirection(current->next->type)
				|| current->next->type == TOK_PIPE
				|| current->next->type == TOK_EOF
				|| current->next->type == TOK_NEW_LINE))
		{
			ft_putendl_fd("\001" RED "\002syntax error near unexpected token 'newline'"RESET, STDERR_FILENO);
			return (false);
		}
		current = current->next;
	}
	return (true);
}

/**
 * @brief Validates token sequence and prints error if invalid
 *
 * @param tokens Token list to validate
 * @return t_bool true if valid sequence, false if invalid
 */
t_bool	validate_token_sequence(t_token *tokens)
{
	if (!tokens)
		return (false);
	if (!check_invalid_tokens(tokens))
		return (false);
	return (true);
}
