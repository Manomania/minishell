/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 15:36:58 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/17 15:37:03 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEBUG_H
# define DEBUG_H

# include "minishell.h"

# define DEBUG_NONE 0
# define DEBUG_ERROR 1
# define DEBUG_INFO 2
# define DEBUG_VERBOSE 3

extern int	g_debug_level;

void	debug_init(int level);
void	debug_log(int level, const char *module, const char *msg);
void	debug_print_token(int level, t_token *token);
void	debug_print_tokens(int level, t_token *tokens);
void	debug_print_command(int level, t_command *cmd);
void	debug_print_commands(int level, t_command *cmd);

#endif
