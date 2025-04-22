/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_checker_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 15:52:57 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/22 19:07:20 by maximart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"

/**
 * @brief Checks for errors with &, ||, and && tokens
 *
 * @param token The token to check
 * @return t_bool false if invalid token found, true otherwise
 */
t_bool	check_logical_tokens(t_token *token)
{
	if (token->type == TOK_ESP)
	{
		ft_putendl_fd("syntax error near unexpected token `&'", STDERR_FILENO);
		return (false);
	}
	if (token->type == TOK_OR)
	{
		ft_putendl_fd("syntax error near unexpected token `||'", STDERR_FILENO);
		return (false);
	}
	if (token->type == TOK_AND)
	{
		ft_putendl_fd("syntax error near unexpected token `&&'", STDERR_FILENO);
		return (false);
	}
	return (true);
}

/**
 * @brief Checks for invalid redirection combinations
 *
 * @param current Current token
 * @return t_bool false if invalid redirection found, true otherwise
 */
t_bool	check_redir_combinations(t_token *current)
{
	if (current->type == TOK_REDIR_FROM && current->next->type == TOK_REDIR_TO)
	{
		ft_putendl_fd("syntax error near unexpected token `newline'",
			STDERR_FILENO);
		return (false);
	}
	if (token_is_redirection(current->type)
		&& current->next->type == TOK_REDIR_TO)
	{
		ft_putendl_fd("syntax error near unexpected token `>'", STDERR_FILENO);
		return (false);
	}
	if (token_is_redirection(current->type)
		&& current->next->type == TOK_REDIR_FROM)
	{
		ft_putendl_fd("syntax error near unexpected token `<'", STDERR_FILENO);
		return (false);
	}
	return (true);
}

/**
 * @brief Checks for invalid pipe token combinations
 *
 * @param current Current token
 * @return t_bool false if invalid pipe found, true otherwise
 */
t_bool	check_pipe_tokens(t_token *current)
{
	if ((current->type == TOK_PIPE && token_is_redirection(current->next->type)
			&& current->next->next->type == TOK_EOF)
		|| (token_is_redirection(current->type)
			&& current->next->type == TOK_PIPE
			&& current->next->next->type == TOK_EOF))
	{
		ft_putendl_fd("syntax error near unexpected token `|'", STDERR_FILENO);
		return (false);
	}
	if (current->type == TOK_PIPE && (current->next->type == TOK_PIPE
			|| current->next->type == TOK_EOF))
	{
		ft_putendl_fd("syntax error near unexpected token `|'", STDERR_FILENO);
		return (false);
	}
	return (true);
}
