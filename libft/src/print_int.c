/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_int.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 10:20:22 by maximart          #+#    #+#             */
/*   Updated: 2025/03/26 17:37:12 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/libft.h"

int	ft_handle_int(int fd, int n)
{
	return (ft_putnbr_fd(n, fd));
}

int	ft_putnbr_unsigned(int fd, unsigned int n)
{
	int	count;

	count = 0;
	if (n > 9)
		count += ft_putnbr_unsigned(fd, n / 10);
	count += ft_putchar_fd(n % 10 + '0', fd);
	return (count);
}

int	ft_handle_nbr_unsigned(int fd, unsigned int n)
{
	return (ft_putnbr_unsigned(fd, n));
}
