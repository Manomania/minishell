/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_is.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 13:58:39 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/08 14:03:25 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Checks if a token is a redirection token
 *
 * @param tok Token type to check
 * @return t_bool true if token is a redirection, false otherwise
 */
t_bool	token_is_redirection(t_token_type type)
{
	return (type == TOK_REDIR_FROM || type == TOK_REDIR_TO
		|| type == TOK_HERE_DOC_FROM || type == TOK_HERE_DOC_TO);
}
