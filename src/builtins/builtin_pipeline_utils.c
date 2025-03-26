/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_pipeline_utils.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 15:20:37 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/26 15:24:08 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Restores original file descriptors
 *
 * This function restores the original standard input and output file
 * descriptors from the saved values, and closes the saved descriptors.
 *
 * @param saved_in Saved stdin file descriptor
 * @param saved_out Saved stdout file descriptor
 */
void	restore_pipeline_fds(int saved_in, int saved_out)
{
	if (saved_in >= 0)
	{
		dup2(saved_in, STDIN_FILENO);
		close(saved_in);
	}
	if (saved_out >= 0)
	{
		dup2(saved_out, STDOUT_FILENO);
		close(saved_out);
	}
}
