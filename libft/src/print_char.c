/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_char.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 10:26:51 by maximart          #+#    #+#             */
/*   Updated: 2025/03/26 18:20:22 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/libft.h"

int	ft_handle_char(int fd, const char c)
{
	return (ft_putchar_fd(c, fd));
}

int	ft_handle_percent(int fd)
{
	return (ft_putchar_fd('%', fd));
}

int	ft_handle_str(int fd, const char *str)
{
	return (ft_putstr_fd(str, fd));
}
