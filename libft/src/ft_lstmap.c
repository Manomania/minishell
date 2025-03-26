/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstmap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maximart <maximart@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 14:51:48 by maximart          #+#    #+#             */
/*   Updated: 2024/11/04 13:43:35 by maximart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/libft.h"

t_list	*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *))
{
	t_list	*modified_lst;
	t_list	*modified_new;
	void	*new_content;

	modified_lst = NULL;
	while (lst)
	{
		new_content = f(lst->content);
		if (!new_content)
		{
			ft_lstclear(&modified_lst, del);
			return (NULL);
		}
		modified_new = ft_lstnew(new_content);
		if (!modified_new)
		{
			del(new_content);
			ft_lstclear(&modified_lst, del);
			return (NULL);
		}
		ft_lstadd_back(&modified_lst, modified_new);
		lst = lst->next;
	}
	return (modified_lst);
}
