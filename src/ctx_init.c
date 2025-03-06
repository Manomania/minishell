/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ctx_init.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 10:56:03 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/06 14:54:41 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_ctx	*ctx_init(void)
{
	t_ctx	*ctx;

	ctx = malloc(sizeof(t_ctx));
	ctx->tokens = NULL;
	return (ctx);
}
