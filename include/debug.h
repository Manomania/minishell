/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 15:36:58 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/28 09:07:46 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEBUG_H
# define DEBUG_H

# include "minishell.h"

// *************************************************************************** #
//                            Function Prototypes                              #
// *************************************************************************** #

// debug.c
void	debug_print_token(t_token *token);
void	debug_print_tokens(t_token *tokens);
void	debug_print_command(t_command *cmd);
void	debug_print_commands(t_command *cmd);
void	debug_log(const char *module, const char *msg);
char	*get_display_value(char *str);

#endif
