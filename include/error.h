/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 15:32:19 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/17 17:45:16 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_H
# define ERROR_H

# include "minishell.h"

// *************************************************************************** #
//                                 Structures                                  #
// *************************************************************************** #

typedef enum e_error_level
{
	INFO,
	WARNING,
	ERROR,
	FATAL
}	t_error_level;

// *************************************************************************** #
//                            Function Prototypes                              #
// *************************************************************************** #

// ctx_error_exit.c
void			ctx_error_exit(t_ctx *ctx, t_error_type err);

// error.c
int				error_code(t_error_type err);
void			error_print(t_error_level level, const char *module, const char *msg);

// error_utils.c
t_error_info	*get_error_table(void);
int				ctx_error(t_error_type err);
int				ctx_error_level(t_error_type err, t_error_level level);
void			error_exit(t_ctx *ctx, t_error_type err, const char *module);

#endif
