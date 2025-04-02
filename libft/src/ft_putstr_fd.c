/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putstr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 14:51:48 by maximart          #+#    #+#             */
/*   Updated: 2025/03/26 18:11:14 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/libft.h"

int	ft_putstr_fd(const char *str, int fd)
{
	if (!str)
		return (ft_putstr_fd("(null)", fd));
	return (write(fd, str, ft_strlen(str)));
}

int	ft_putstr(const char *str)
{
	return (ft_putstr_fd(str, STDOUT_FILENO));
}
