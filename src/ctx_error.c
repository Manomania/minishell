/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ctx_error.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 16:13:34 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/08 12:18:27 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Gets the error table object
 *
 * @return t_error_info* the error table map
 */
static t_error_info	*get_error_table(void)
{
	t_error_info	*error_table;

	error_table = malloc(sizeof(t_error_info));
	error_table[ERR_NONE] = (t_error_info){0, "Success", false};
	error_table[ERR_CMD_NOT_FOUND] = (t_error_info){ENOENT, "Command not found",
		false};
	error_table[ERR_NO_PERMISSION] = (t_error_info){EACCES, "Permission denied",
		true};
	error_table[ERR_IO_ERROR] = (t_error_info){EIO, "Input/output error", true};
	error_table[ERR_UNIMPLEMENTED] = (t_error_info){ENOSYS,
		"Not implemented yet", true};
	return (error_table);
}

/**
 * @brief Displays an error and gets an exit code
 *
 * @param err error type
 * @return int exit code
 */
int	ctx_error(t_error_type err)
{
	t_error_info	*info;
	t_error_info	*error_table;

	error_table = get_error_table();
	info = &error_table[err];
	if (info->use_perror)
		perror(info->message);
	else
		ft_putstr_fd(ft_strjoin("Error: ", info->message), 2);
	return (info->code);
}
