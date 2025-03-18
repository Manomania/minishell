/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ctx_error_exit.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 17:07:35 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/17 17:45:59 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"

/**
 * @brief Frees context, displays an error and quits with the corresponding
 * exit code
 *
 * @param ctx context
 * @param err error code
 */
void	ctx_error_exit(t_ctx *ctx, t_error_type err)
{
	if (ctx)
		ctx_clear(ctx);
	errno = ctx_error(err);
	exit(errno);
}
