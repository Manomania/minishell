/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 14:51:48 by maximart          #+#    #+#             */
/*   Updated: 2025/03/26 17:38:46 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/libft.h"

int	ft_putnbr_fd(int n, int fd)
{
	char	num;
	int		count;

	count = 0;
	if (n == -2147483648)
		return (write(fd, "-2147483648", 11));
	if (n < 0)
	{
		count += write(fd, "-", 1);
		n = -n;
	}
	if (n > 9)
		count += ft_putnbr_fd((n / 10), fd);
	num = (n % 10) + '0';
	count += write(fd, &num, 1);
	return (count);
}
