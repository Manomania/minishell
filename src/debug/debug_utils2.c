/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_utils2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 11:34:34 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/27 17:47:57 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "debug.h"
#include "minishell.h"

/**
 * @brief Debug utility to display the current exit status
 *
 * @param ctx Context containing the shell state
 */
void	debug_exit_status(t_ctx *ctx)
{
	char	buffer[64];

	ft_strlcpy(buffer, "Exit status: ", sizeof(buffer));
	ft_strlcat(buffer, ft_itoa(ctx->exit_status), sizeof(buffer));
	debug_log("exit_status", buffer);
}

/**
 * @brief Gets the string representation of a token type
 *
 * @param type Token type
 * @return String representation of the token type
 */
char	*get_token_type_str(t_token_type type)
{
	if (type == TOK_WORD)
		return ((char *)"WORD");
	else if (type == TOK_REDIR_FROM)
		return ((char *)"REDIR_FROM");
	else if (type == TOK_REDIR_TO)
		return ((char *)"REDIR_TO");
	else if (type == TOK_HERE_DOC_FROM)
		return ((char *)"HERE_DOC_FROM");
	else if (type == TOK_HERE_DOC_TO)
		return ((char *)"HERE_DOC_TO");
	else if (type == TOK_PIPE)
		return ((char *)"PIPE");
	else if (type == TOK_NEW_LINE)
		return ((char *)"NEW_LINE");
	else if (type == TOK_EOF)
		return ((char *)"EOF");
	return ((char *)"UNKNOWN");
}

/**
 * @brief Prints the redirection type lol
 *
 * @param type Redirection type
 */
void	print_redirection_type(t_token_type type)
{
	if (type == TOK_REDIR_FROM)
		ft_printf("< (REDIR_FROM)");
	else if (type == TOK_REDIR_TO)
		ft_printf("> (REDIR_TO)");
	else if (type == TOK_HERE_DOC_FROM)
		ft_printf("<< (HERE_DOC_FROM)");
	else if (type == TOK_HERE_DOC_TO)
		ft_printf(">> (HERE_DOC_TO)");
	else
		ft_printf("UNKNOWN");
}
