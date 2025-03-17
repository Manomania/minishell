/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ctx_error.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 16:13:34 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/17 17:45:36 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"

/**
 * @brief Gets the error table object
 *
 * @return t_error_info* The error table map
 */
t_error_info	*get_error_table(void)
{
	t_error_info	*error_table;

	error_table = malloc(32 * sizeof(t_error_info));
	error_table[ERR_NONE] = (t_error_info){0, "Success", false};
	error_table[ERR_CMD_NOT_FOUND] = (t_error_info){ENOENT, "Command not found",
		false};
	error_table[ERR_NO_PERMISSION] = (t_error_info){EACCES, "Permission denied",
		true};
	error_table[ERR_IO_ERROR] = (t_error_info){EIO, "Input/output error", true};
	error_table[ERR_UNIMPLEMENTED] = (t_error_info){ENOSYS,
		"Not implemented yet", true};
	error_table[ERR_ALLOC] = (t_error_info){ENOMEM,
		"Allocation failed. Your RAM might be full", true};
	error_table[ERR_PIPE] = (t_error_info){EPIPE, "Pipe error", false};
	error_table[ERR_CHILD] = (t_error_info){ECHILD, "Fork error", false};
	return (error_table);
}

/**
 * @brief Displays an error and gets an exit code
 *
 * @param err Error type
 * @return int Exit code
 */
int	ctx_error(t_error_type err)
{
	return (ctx_error_level(err, ERROR));
}

/**
 * @brief Displays an error and gets an exit code
 *
 * @param err Error type
 * @param level Error level
 * @return int Exit code
 */
int	ctx_error_level(t_error_type err, t_error_level level)
{
	t_error_info	*error_table;
	t_error_info	*info;
	char			*msg;
	int				code;

	error_table = get_error_table();
	info = &error_table[err];
	msg = (char *)info->message;
	if (info->use_perror)
		msg = strerror(info->code);
	error_print(level, "minishell", msg);
	code = info->code;
	free(error_table);
	return (code);
}
