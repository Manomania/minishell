/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maximart <maximart@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 14:51:48 by maximart          #+#    #+#             */
/*   Updated: 2024/11/04 16:47:31 by maximart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/libft.h"

void	*ft_memmove(void *dest, const void *src, size_t n)
{
	unsigned char		*dest_data;
	unsigned const char	*src_data;

	if (!dest && !src)
		return (NULL);
	if (src > dest)
		dest = ft_memcpy(dest, src, n);
	else
	{
		dest_data = (unsigned char *)dest;
		src_data = (unsigned const char *)src;
		while (n > 0)
		{
			n--;
			dest_data[n] = src_data[n];
		}
	}
	return (dest);
}
