/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 17:20:00 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/16 15:29:32 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"

/**
 * @brief Formats error message for commands not found
 *
 * @param proof Command name
 * @param msg Error message
 * @return int Error code (127 for command not found)
 */
static int	handle_cmd_not_found(const char *proof, const char *msg)
{
	ft_printf_fd(STDERR_FILENO, "minishell: %s: %s\n", proof, msg);
	if (ft_strncmp(msg, "No such file or directory", 25) == 0)
		return (127);
	else if (ft_strncmp(msg, "Permission denied", 17) == 0)
		return (126);
	else if (ft_strncmp(msg, "Is a directory", 14) == 0)
		return (126);
	return (1);
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
 * @brief Get numeric error code and handle special command not found case
 *
 * @param proof Command name or other proof
 * @param module Module name
 * @param err Error type
 * @return int Error code
 */
int	error(const char *proof, const char *module, t_error_type err)
{
	t_error_info	*error_table;
	t_error_info	*info;
	int				code;

	error_table = get_error_table();
	info = &error_table[err];
	if (err == ERR_CMD_NOT_FOUND && proof)
		code = handle_cmd_not_found(proof, info->message);
	else
	{
		error_print(proof, module, info->message);
		code = info->code;
	}
	free(error_table);
	return (code);
}
