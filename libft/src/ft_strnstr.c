/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maximart <maximart@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 14:51:48 by maximart          #+#    #+#             */
/*   Updated: 2024/11/04 13:43:35 by maximart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/libft.h"

char	*ft_strnstr(const char *big, const char *little, size_t len)
{
	size_t	i;
	size_t	j;
	size_t	length;

	if (!little[0])
		return ((char *)big);
	i = 0;
	length = ft_strlen(little);
	while (big[i] && (i + length - 1) < len)
	{
		j = 0;
		while (little[j] && big[i + j] == little[j])
		{
			if (j == length - 1)
				return ((char *)(big + i));
			j++;
		}
		i++;
	}
	return (0);
}
