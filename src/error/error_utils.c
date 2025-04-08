/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/19 10:35:19 by maximart          #+#    #+#             */
/*   Updated: 2025/04/08 17:02:50 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"

/**
 * @brief Displays an error message to STDERR
 *
 * @param proof (optional) Proof that the error occured
 * @param module (optional) Module where the error occured
 * @param msg Error message
 */
void	error_print(const char *proof, const char *module, const char *msg)
{
	if (module && proof)
		ft_printf_fd(STDERR_FILENO,
			"\001" RED "\002"
			"minishell: %s: `%s' %s\n" RESET,
			module,
			proof,
			msg);
	else if (module)
		ft_printf_fd(STDERR_FILENO,
			"\001" RED "\002"
			"minishell: %s: %s\n" RESET,
			module,
			msg);
	else if (proof)
		ft_printf_fd(STDERR_FILENO,
			"\001" RED "\002"
			"minishell: `%s' %s\n" RESET,
			proof,
			msg);
	else
		ft_printf_fd(STDERR_FILENO,
			"\001" RED "\002"
			"minishell: %s\n" RESET,
			msg);
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
	error_table[ERR_CMD_NOT_FOUND] = (t_error_info){127, "command not found"};
	error_table[ERR_NO_PERMS] = (t_error_info){126, "permission denied"};
	error_table[ERR_NO_FILE] = (t_error_info){ENOENT,
		"no such file or directory"};
	error_table[ERR_IS_DIR] = (t_error_info){126, "is a directory"};
	error_table[ERR_NO_PWD] = (t_error_info){1,
		"failed to get current directory"};
	error_table[ERR_NO_OLDPWD] = (t_error_info){1, "OLDPWD not set"};
	error_table[ERR_NO_HOME] = (t_error_info){1, "HOME not set"};
	error_table[ERR_VLD_INPUT_LENGTH] = (t_error_info){1,
		"input exceeds maximum length"};
	error_table[ERR_VLD_ENV_VAR_EMPTY] = (t_error_info){1,
		"empty variable name"};
	error_table[ERR_VLD_ENV_VAR_START] = (t_error_info){1,
		"variable name must start with letter or _"};
	error_table[ERR_VLD_ENV_VAR_BAD_CHAR] = (t_error_info){1,
		"invalid character in variable name"};
	error_table[ERR_VLD_ENV_VAR_TOO_LONG] = (t_error_info){1,
		"variable name too long"};
	error_table[ERR_VLD_REDIR_FILENAME_EMPTY] = (t_error_info){1,
		"empty filename"};
	error_table[ERR_VLD_REDIR_FILENAME_TOO_LONG] = (t_error_info){1,
		"filename too long"};
	error_table[ERR_IO] = (t_error_info){EIO, "i/o error"};
	error_table[ERR_FD] = (t_error_info){1, "bad file descriptor"};
	error_table[ERR_ALLOC] = (t_error_info){ENOMEM, "allocation failed"};
	error_table[ERR_CHILD] = (t_error_info){ECHILD, "child/fork/execve error"};
	error_table[ERR_PIPE] = (t_error_info){EPIPE, "pipe processing failed"};
	error_table[ERR_IDENTIFIER] = (t_error_info){1, "not a valid identifier"};
	error_table[ERR_NUMERIC] = (t_error_info){255, "numeric argument required"};
	error_table[ERR_TOO_MANY_ARGS] = (t_error_info){1, "too many arguments"};
	error_table[ERR_UNCLOSED_QUOTE] = (t_error_info){1, "unclosed quote"};
	error_table[ERR_TOKEN_LIST] = (t_error_info){1,
		"failed to build token list"};
	return (error_table);
}
