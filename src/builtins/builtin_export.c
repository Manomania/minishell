/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 09:55:00 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/21 10:02:44 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "debug.h"
#include "error.h"
#include "minishell.h"
#include "validation.h"

/**
 * @brief Checks if a character is valid for env variable name
 *
 * @param c Character to check
 * @return t_bool true if valid, false otherwise
 */
static t_bool	is_valid_env_char(char c)
{
	return (ft_isalnum(c) || c == '_');
}

/**
 * @brief Gets the value part of an environment variable
 *
 * @param arg Argument string (key=value)
 * @return char* Value part or NULL if no value
 */
static char	*get_env_value(char *arg)
{
	char	*equals;

	equals = ft_strchr(arg, '=');
	if (!equals)
		return (NULL);
	return (equals + 1);
}

/**
 * @brief Gets the key part of an environment variable
 *
 * @param arg Argument string (key=value)
 * @return char* Key part (caller must free)
 */
static char	*get_env_key(char *arg)
{
	char	*equals;
	char	*key;
	int		key_len;

	equals = ft_strchr(arg, '=');
	if (!equals)
		key_len = ft_strlen(arg);
	else
		key_len = equals - arg;
	key = malloc(key_len + 1);
	if (!key)
		return (NULL);
	ft_strlcpy(key, arg, key_len + 1);
	return (key);
}

/**
 * @brief Prints all environment variables in export format
 *
 * @param ctx Context for shell environment
 * @return void
 */
static void	print_export_env(t_ctx *ctx)
{
	t_env	*env;

	env = ctx->env_list;
	while (env)
	{
		ft_printf("declare -x %s", env->key);
		if (env->value)
			ft_printf("=\"%s\"", env->value);
		ft_printf("\n");
		env = env->next;
	}
}

/**
 * @brief Updates an existing environment variable
 *
 * @param env_list Environment list
 * @param key Key to update
 * @param value New value
 * @return t_bool true if updated, false if not found
 */
static t_bool	update_env_var(t_env *env_list, char *key, char *value)
{
	t_env	*current;

	current = env_list;
	while (current)
	{
		if (ft_strncmp(current->key, key, ft_strlen(key) + 1) == 0)
		{
			if (value)
			{
				if (current->value)
					free(current->value);
				current->value = ft_strdup(value);
			}
			return (true);
		}
		current = current->next;
	}
	return (false);
}

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
