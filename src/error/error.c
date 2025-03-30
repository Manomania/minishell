/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 17:20:00 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/28 09:16:10 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"

/**
 * @brief Get numeric error code from error type
 *
 * @param err Error type
 * @return int Integer error code
 */
int	error_code(t_error_type err)
{
	t_error_info	*info;
	t_error_info	*error_table;
	int				code;

	error_table = get_error_table();
	info = &error_table[err];
	code = info->code;
	free(error_table);
	return (code);
}
