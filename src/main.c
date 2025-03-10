/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maximart <maximart@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 17:19:32 by maximart          #+#    #+#             */
/*   Updated: 2025/03/04 17:19:34 by maximart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// int	main(void)
// {
//
// 	return (0);
// }

// // t_token	*parse_line(t_ctx ctx, char *line)
// // {
// //
// // }
//
// /*
// // Redirections
// <
// >
//
// // Here_doc
// <<
// >>
//
// // Pipes
// |
//
// // Env
// $ENV
// $?
//
// // Logical operations
// &&
// ||
//
// // Wildcards (no glob)
// *
// hello/*
// hello*
//
// // Le reste: tokens de programme
// echo hello
// echo "hello|"
//
// // Example
// ```sh
// bash-5.1$ echo "hello 'world "shrek" swamp' bitch"
// hello 'world shrek swamp' bitch
// bash-5.1$ echo "hello 'world "sh|rek" swamp' bitch"
// bash: rek swamp' bitch: command not found
// bash-5.1$ echo "hello 'worl|d "shrek" swamp' bitch"
// hello 'worl|d shrek swamp' bitch
// ```
//
// // La regle d'or:
// - Quand on rentre dans un simple/double quote, on attend la fin
// du simple/double quote, c'est tout.
// */