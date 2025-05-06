/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 15:32:19 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/06 13:48:23 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_H
# define ERROR_H

# include "minishell.h"

// *************************************************************************** #
//                            Function Prototypes                              #
// *************************************************************************** #

// ctx_error_exit.c
void			ctx_error_exit(t_ctx *ctx, const char *proof,
					const char *module, t_error_type err);

// error.c
int				error_code(t_error_type err);

// error_utils.c
t_error_info	*get_error_table(void);
int				error(const char *proof, const char *module, t_error_type err);

// error_utils2.c
void			error_print(const char *proof, const char *module,
					const char *msg);

// ctx_exit.c
void			ctx_exit(t_ctx *ctx);

#endif
