/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 15:34:04 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/17 15:34:09 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "memory.h"

/**
 * @brief Safely allocates memory with error handling
 *
 * @param ctx Shell context for error handling
 * @param size Size to allocate
 * @param module Module name for error reporting
 * @return Allocated memory or exits on failure
 */
void	*safe_malloc(t_ctx *ctx, size_t size, const char *module)
{
	void	*ptr;

	ptr = malloc(size);
	if (!ptr)
	{
		error_print(FATAL, module, "Memory allocation failed");
		error_exit(ctx, ERR_ALLOC, module);
	}
	return (ptr);
}

/**
 * @brief Safely duplicates a string with error handling
 *
 * @param ctx Shell context for error handling
 * @param str String to duplicate
 * @param module Module name for error reporting
 * @return Duplicated string or exits on failure
 */
char	*safe_strdup(t_ctx *ctx, const char *str, const char *module)
{
	char	*dup;

	if (!str)
		return (NULL);
	dup = ft_strdup(str);
	if (!dup)
	{
		error_print(FATAL, module, "String duplication failed");
		error_exit(ctx, ERR_ALLOC, module);
	}
	return (dup);
}

/**
 * @brief Safely joins two strings with error handling
 *
 * @param ctx Shell context for error handling
 * @param s1 First string
 * @param s2 Second string
 * @param module Module name for error reporting
 * @return Joined string or exits on failure
 */
char	*safe_strjoin(t_ctx *ctx, const char *s1, const char *s2,
		const char *module)
{
	char	*result;

	result = ft_strjoin(s1, s2);
	if (!result)
	{
		error_print(FATAL, module, "String join failed");
		error_exit(ctx, ERR_ALLOC, module);
	}
	return (result);
}

/**
 * @brief Safely allocates and zeros memory with error handling
 *
 * @param ctx Shell context for error handling
 * @param nmemb Number of elements
 * @param size Size of each element
 * @param module Module name for error reporting
 * @return Allocated memory or exits on failure
 */
void	*safe_calloc(t_ctx *ctx, size_t nmemb, size_t size, const char *module)
{
	void	*ptr;

	ptr = ft_calloc(nmemb, size);
	if (!ptr)
	{
		error_print(FATAL, module, "Memory allocation failed");
		error_exit(ctx, ERR_ALLOC, module);
	}
	return (ptr);
}
