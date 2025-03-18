/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 15:39:38 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/17 15:49:51 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "validation.h"

/**
 * @brief Validates that the input string is not too long
 *
 * @param input The input string to validate
 * @param ctx Context for error handling
 * @return t_bool true if valid, false if invalid
 */
t_bool	validate_input_length(char *input, t_ctx *ctx)
{
	size_t	len;

	(void)ctx;
	if (!input)
		return (false);
	len = ft_strlen(input);
	if (len > MAX_INPUT_LEN)
	{
		error_print(ERROR, "input", "Input exceeds maximum length");
		return (false);
	}
	return (true);
}

/**
 * @brief Validates that an environment variable name is valid
 *
 * @param name The variable name to validate
 * @param ctx Context for error handling
 * @return t_bool true if valid, false if invalid
 */
t_bool	validate_env_var_name(char *name, t_ctx *ctx)
{
	int	i;

	(void)ctx;
	if (!name || !name[0])
	{
		error_print(ERROR, "env", "Empty variable name");
		return (false);
	}
	if (!ft_isalpha(name[0]) && name[0] != '_')
	{
		error_print(ERROR, "env", "Variable name must start with letter or _");
		return (false);
	}
	i = 1;
	while (name[i])
	{
		if (!ft_isalnum(name[i]) && name[i] != '_')
		{
			error_print(ERROR, "env", "Invalid character in variable name");
			return (false);
		}
		i++;
	}
	if (ft_strlen(name) > MAX_VAR_NAME_LEN)
	{
		error_print(ERROR, "env", "Variable name too long");
		return (false);
	}
	return (true);
}

/**
 * @brief Validates a filename for redirections
 *
 * @param filename The filename to validate
 * @param ctx Context for error handling
 * @return t_bool true if valid, false if invalid
 */
t_bool	validate_filename(char *filename, t_ctx *ctx)
{
	(void)ctx;
	if (!filename || !filename[0])
	{
		error_print(ERROR, "redir", "Empty filename");
		return (false);
	}
	if (ft_strlen(filename) > 255)
	{
		error_print(ERROR, "redir", "Filename too long");
		return (false);
	}
	return (true);
}

/**
 * @brief Validates a full command structure
 *
 * @param cmd The command to validate
 * @param ctx Context for error handling
 * @return t_bool true if valid, false if invalid
 */
t_bool	validate_command(t_command *cmd, t_ctx *ctx)
{
	t_redirection	*redir;

	if (!cmd)
		return (false);
	redir = cmd->redirection;
	while (redir)
	{
		if (!validate_filename(redir->filename, ctx))
			return (false);
		redir = redir->next;
	}
	return (true);
}
