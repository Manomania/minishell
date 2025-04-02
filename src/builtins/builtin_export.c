/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 09:55:00 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/28 11:12:26 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "debug.h"
#include "error.h"
#include "minishell.h"
#include "validation.h"

/**
 * @brief Validates environment variable name
 *
 * @param key Key to validate
 * @return t_bool true if valid, false otherwise
 */
static t_bool	validate_env_key(char *key)
{
	int	i;

	if (!key || !key[0])
		return (false);
	if (!ft_isalpha(key[0]) && key[0] != '_')
		return (false);
	i = 1;
	while (key[i])
	{
		if (!is_valid_env_char(key[i]))
			return (false);
		i++;
	}
	return (true);
}

/**
 * @brief Gets the value part of an environment variable
 *
 * @param arg Export argument in form key=value
 * @return char* Value part (caller must free) or NULL if no value
 */
static char	*get_env_value_from_export(char *arg)
{
	char	*equals;

	equals = ft_strchr(arg, '=');
	if (!equals)
		return (NULL);
	return (ft_strdup(equals + 1));
}

/**
 * @brief Processes a single export argument
 *
 * @param ctx Context containing environment
 * @param arg Argument to process
 * @return int 0 on success, 1 on error
 */
static int	process_export_arg(t_ctx *ctx, char *arg)
{
	char	*key;
	char	*value;
	t_bool	has_equals;
	int		result;

	key = get_env_key_from_export(arg);
	if (!key)
		return (1);
	if (!validate_env_key(key))
	{
		(void)error(arg, "export", ERR_IDENTIFIER);
		free(key);
		return (1);
	}
	has_equals = (ft_strchr(arg, '=') != NULL);
	value = NULL;
	if (has_equals)
		value = get_env_value_from_export(arg);
	if (!update_env_var(&ctx->env_list, key, value, has_equals))
		result = add_env_var(&ctx->env_list, key, value);
	else
		result = 1;
	free(key);
	if (value)
		free(value);
	if (result)
		return (0);
	return (1);
}

/**
 * @brief Executes the export built-in command
 *
 * @param ctx Context for shell environment
 * @param cmd Command containing arguments
 * @return int Exit status (0 for success, non-zero for error)
 */
int	builtin_export(t_ctx *ctx, t_command *cmd)
{
	int	i;
	int	status;

	if (cmd->arg_count == 0)
	{
		print_export_env(ctx);
		return (0);
	}
	status = 0;
	i = 1;
	while (i <= cmd->arg_count)
	{
		if (process_export_arg(ctx, cmd->args[i]) != 0)
			status = 1;
		i++;
	}
	return (status);
}
