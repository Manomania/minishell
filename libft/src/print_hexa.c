/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_hexa.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/11 13:27:39 by maximart          #+#    #+#             */
/*   Updated: 2025/03/26 17:34:08 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/libft.h"

int	ft_puthexa(int fd, unsigned int n, int upper)
{
	int			count;
	const char	*base;

	if (upper)
		base = "0123456789abcdef";
	else
		base = "0123456789ABCDEF";
	count = 0;
	if (n >= 16)
		count += ft_puthexa(fd, n / 16, upper);
	count += ft_putchar_fd(base[n % 16], fd);
	return (count);
}

int	ft_handle_hexa(int fd, unsigned int n, t_format fmt)
{
	int	count;

	count = 0;
	if (fmt.conversion == 'x')
		count += ft_puthexa(fd, n, 1);
	else if (fmt.conversion == 'X')
		count += ft_puthexa(fd, n, 0);
	return (count);
}
