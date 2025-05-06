/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checker.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 13:41:01 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/06 13:54:18 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHECKER_H
# define CHECKER_H

# include "minishell.h"

// *************************************************************************** #
//                            Function Prototypes                              #
// *************************************************************************** #

// token_checker.c
t_bool						validate_token_sequence(t_token *tokens);
void						print_token_error(const char *s);

// token_checker_utils.c
t_bool						check_pipe_tokens(t_token *current);
t_bool						check_redir_combinations(t_token *current);
t_bool						check_logical_tokens(t_token *token);

#endif
