/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 17:20:00 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/17 18:19:26 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"

/**
 * @brief Get color code based on error level
 *
 * @param level Error severity level
 * @return Corresponding color code string
 */
static const char	*get_level_color(t_error_level level)
{
	if (level == INFO)
		return (BLUE);
	if (level == WARNING)
		return (YELLOW);
	if (level == ERROR)
		return (RED);
	if (level == FATAL)
		return (MAGENTA);
	return (RESET);
}

/**
 * @brief Get text representation of error level
 *
 * @param level Error severity level
 * @return Text description of the level
 */
static const char	*get_level_text(t_error_level level)
{
	if (level == INFO)
		return ("INFO");
	if (level == WARNING)
		return ("WARNING");
	if (level == ERROR)
		return ("ERROR");
	if (level == FATAL)
		return ("FATAL");
	return ("UNKNOWN");
}

/**
 * @brief Print an error message with appropriate formatting
 *
 * @param level Error severity level
 * @param module Name of the module where error occurred
 * @param msg Error message to display
 */
void	error_print(t_error_level level, const char *module, const char *msg)
{
	const char	*color;
	const char	*level_text;

	color = get_level_color(level);
	level_text = get_level_text(level);
	ft_putstr_fd((char *)color, STDERR_FILENO);
	ft_putstr_fd((char *)"[", STDERR_FILENO);
	ft_putstr_fd((char *)level_text, STDERR_FILENO);
	ft_putstr_fd((char *)"] ", STDERR_FILENO);
	if (module)
	{
		ft_putstr_fd((char *)module, STDERR_FILENO);
		ft_putstr_fd((char *)": ", STDERR_FILENO);
	}
	ft_putstr_fd((char *)msg, STDERR_FILENO);
	ft_putstr_fd((char *)RESET, STDERR_FILENO);
	ft_putstr_fd((char *)"\n", STDERR_FILENO);
}

/**
 * @brief Print a system error message using errno
 *
 * @param level Error severity level
 * @param module Name of the module where error occurred
 */
void	error_print_sys(t_error_level level, const char *module)
{
	error_print(level, module, strerror(errno));
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
