/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals_update.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 13:27:13 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/06 18:16:17 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "globals.h"
#include "minishell.h" // required bc globals.h does not include minishell.h

void	sig_interactive_handler(int sig);

/////////////////////////////// Interactive mode ///////////////////////////////
