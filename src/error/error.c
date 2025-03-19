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
