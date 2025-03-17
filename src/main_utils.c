/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 17:47:37 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/17 17:50:46 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"
#include "validation.h"

/**
 * @brief Creates a prompt string based on previous command status
 *
 * @param prev_status Exit status of previous command
 * @return Formatted prompt string
 */
char	*create_prompt(int prev_status)
{
	char	*rdl_str1;
	char	*rdl_str2;
	char	*rdl_str3;
	char	*prompt;

	prompt = NULL;
	if (prev_status > 0)
	{
		rdl_str1 = ft_itoa(prev_status);
		if (!rdl_str1)
			return (NULL);
		rdl_str2 = ft_strjoin("\001" RED "\002", rdl_str1);
		free(rdl_str1);
		if (!rdl_str2)
			return (NULL);
		rdl_str3 = ft_strjoin(rdl_str2, " $ \001" RESET "\002");
		free(rdl_str2);
		prompt = rdl_str3;
	}
	else
		prompt = ft_strdup("\001" GREEN "\002$ \001" RESET "\002");
	return (prompt);
}

/**
 * @brief Gets user input with appropriate prompt
 *
 * @param ctx Shell context
 * @param prev_status Exit status of previous command
 * @return User input string or NULL on error/EOF
 */
char	*get_user_input(t_ctx *ctx, int prev_status)
{
	char	*prompt;
	char	*input;

	prompt = create_prompt(prev_status);
	if (!prompt)
		error_exit(ctx, ERR_ALLOC, "prompt");
	input = readline(prompt);
	free(prompt);
	if (!input)
	{
		ft_putstr("exit\n");
		return (NULL);
	}
	if (input[0] != '\0')
		add_history(input);
	if (!validate_input_length(input, ctx))
	{
		free(input);
		return (NULL);
	}
	return (input);
}
