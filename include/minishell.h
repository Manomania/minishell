/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 17:15:54 by maximart          #+#    #+#             */
/*   Updated: 2025/03/05 11:32:25 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft.h"
# include <stdlib.h>
/*******************************************************************************
*                                    Macros                                    *
*******************************************************************************/

# define RESET	"\033[039m"
# define RED	"\033[091m"
# define GREEN	"\033[092m"
# define YELLOW	"\033[093m"

/*******************************************************************************
*                                  Structures                                  *
*******************************************************************************/

typedef enum e_token_type
{
	TOK_COMMAND,
	TOK_REDIR_FROM,
	TOK_REDIR_TO,
	TOK_HERE_DOC_FROM,
	TOK_HERE_DOC_TO,
	TOK_PIPE,
	TOK_ENV,
	TOK_AND,
	TOK_OR,
}	t_token_type;

typedef struct s_token
{
	t_token_type	type;
	char			*value;
}					t_token;

typedef struct s_lexer
{
	char	*input;
	int		position;
	int		length;
}			t_lexer;

typedef struct s_ctx
{
}			t_ctx;

/*******************************************************************************
*                             Function Prototypes                              *
*******************************************************************************/

#endif
