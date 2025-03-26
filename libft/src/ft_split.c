/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maximart <maximart@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 14:51:48 by maximart          #+#    #+#             */
/*   Updated: 2024/11/04 13:43:35 by maximart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/libft.h"

size_t	count_words(const char *s, char c)
{
	size_t	count;
	int		in_word;

	count = 0;
	in_word = 0;
	while (*s)
	{
		if (*s != c && in_word == 0)
		{
			in_word = 1;
			count++;
		}
		else if (*s == c)
			in_word = 0;
		s++;
	}
	return (count);
}

static char	*malloc_word(const char *s, size_t start, size_t end)
{
	char	*word;
	size_t	i;

	word = ft_calloc(sizeof(char), (end - start + 1));
	if (!word)
		return (NULL);
	i = 0;
	while (start < end)
		word[i++] = s[start++];
	return (word);
}

void	free_words(char **words, size_t i)
{
	while (i > 0)
		free(words[--i]);
	free(words);
}

static char	**ft_fill_split(char const *s, char c, char **split, size_t word_c)
{
	size_t	i;
	size_t	j;
	size_t	start;

	i = 0;
	j = 0;
	while (s[i] && j < word_c)
	{
		while (s[i] && s[i] == c)
			i++;
		start = i;
		while (s[i] && s[i] != c)
			i++;
		if (i > start)
		{
			split[j] = malloc_word(s, start, i);
			if (!split[j++])
				return (free_words(split, j), NULL);
		}
	}
	split[j] = NULL;
	return (split);
}

char	**ft_split(char const *s, char c)
{
	char	**split;
	size_t	words_count;

	if (!s)
		return (NULL);
	words_count = count_words(s, c);
	split = (char **)ft_calloc(sizeof(char *), (words_count + 1));
	if (!split)
		return (NULL);
	return (ft_fill_split(s, c, split, words_count));
}
