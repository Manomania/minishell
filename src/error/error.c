/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 17:20:00 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/08 18:13:51 by elagouch         ###   ########.fr       */
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
	ft_putstr_fd("minishell: ", STDOUT_FILENO);
	ft_putstr_fd((char *)proof, STDERR_FILENO);
	ft_putstr_fd(": ", STDERR_FILENO);
	ft_putstr_fd((char *)msg, STDERR_FILENO);
	ft_putstr_fd("\n", STDERR_FILENO);
	return (127);
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
