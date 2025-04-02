/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_adress.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 14:16:35 by maximart          #+#    #+#             */
/*   Updated: 2025/03/26 17:29:02 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/libft.h"

int	ft_printmemory(int fd, unsigned long n)
{
	int			count;
	const char	*base;

	base = "0123456789abcdef";
	count = 0;
	if (n >= 16)
		count += ft_printmemory(fd, n / 16);
	count += ft_putchar_fd(base[n % 16], fd);
	return (count);
}

int	ft_putptr(int fd, unsigned long ptr)
{
	int	count;

	count = 0;
	if (!ptr)
		return (ft_putstr_fd("(nil)", fd));
	count += ft_putstr_fd("0x", fd);
	count += ft_printmemory(fd, ptr);
	return (count);
}

int	ft_handle_ptr(int fd, unsigned long ptr)
{
	return (ft_putptr(fd, ptr));
}
