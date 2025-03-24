/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 09:55:00 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/24 12:37:57 by elagouch         ###   ########.fr       */
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
 * @brief Processes a single export argument
 *
 * @param ctx Context for shell environment
 * @param arg Argument to process
 * @return int 0 on success, 1 on error
 */
static int	process_export_arg(t_ctx *ctx, char *arg)
{
	char	*key;
	char	*value;
	int		result;

	key = get_env_key(arg);
	if (!key)
		return (1);
	if (!validate_env_key(key))
	{
		ft_putstr_fd((char *)"minishell: export: `", STDERR_FILENO);
		ft_putstr_fd(arg, STDERR_FILENO);
		ft_putstr_fd((char *)"': not a valid identifier\n", STDERR_FILENO);
		free(key);
		return (1);
	}
	value = get_env_value(arg);
	if (!update_env_var(ctx->env_list, key, value))
	{
		result = add_env_var(&ctx->env_list, key, value);
		free(key);
		return (!!result);
	}
	free(key);
	return (0);
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

	debug_log(DEBUG_INFO, "builtin", "Executing export builtin");
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
