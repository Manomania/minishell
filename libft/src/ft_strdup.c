/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maximart <maximart@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 14:51:48 by maximart          #+#    #+#             */
/*   Updated: 2024/11/04 13:43:35 by maximart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/libft.h"

char	*ft_strdup(const char *s)
{
	char	*dup;
	int		s_len;

	s_len = ft_strlen(s);
	dup = (char *)malloc(sizeof(char) * (s_len + 1));
	if (!dup)
		return (NULL);
	dup[s_len] = 0;
	while (s_len >= 0)
	{
		dup[s_len] = s[s_len];
		s_len--;
	}
	return (dup);
}
