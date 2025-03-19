/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maximart <maximart@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/19 10:35:19 by maximart          #+#    #+#             */
/*   Updated: 2025/03/19 10:35:22 by maximart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"

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
 * @brief Gets the error table object
 *
 * @return t_error_info* The error table map
 */
t_error_info	*get_error_table(void)
{
	t_error_info	*error_table;

	error_table = malloc(32 * sizeof(t_error_info));
	error_table[ERR_NONE] = (t_error_info){0, "Success", false};
	error_table[ERR_CMD_NOT_FOUND] = (t_error_info){127, "Command not found",
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
	error_table[ERR_NO_SUCH_FILE] = (t_error_info){ENOENT,
		"No such file or directory", false};
	return (error_table);
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

/**
 * @brief Print a system error message using errno
 *
 * @param level Error severity level
 * @param module Name of the module where error occurred
 */
static void	error_print_sys(t_error_level level, const char *module)
{
	error_print(level, module, strerror(errno));
}

/**
 * @brief Exit the program with an error message
 *
 * @param ctx Context to clean up
 * @param err Error type to report
 * @param module Name of the module where error occurred
 */
void	error_exit(t_ctx *ctx, t_error_type err, const char *module)
{
	t_error_info	*info;
	t_error_info	*error_table;
	int				code;

	if (ctx)
		ctx_clear(ctx);
	error_table = get_error_table();
	info = &error_table[err];
	if (info->use_perror)
		error_print_sys(FATAL, module);
	else
		error_print(FATAL, module, info->message);
	code = info->code;
	free(error_table);
	exit(code);
}
