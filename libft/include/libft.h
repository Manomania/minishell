/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 14:51:48 by maximart          #+#    #+#             */
/*   Updated: 2025/03/26 17:47:14 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_H
# define LIBFT_H

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1
# endif

# ifndef OPEN_MAX
#  define OPEN_MAX 1024
# endif

# include <fcntl.h>
# include <stdarg.h>
# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>

typedef struct s_list
{
	void			*content;
	struct s_list	*next;
}					t_list;

typedef struct s_format
{
	char			conversion;
}					t_format;

int					ft_putnbr(int n);
int					ft_toupper(int c);
int					ft_tolower(int c);
int					ft_isprint(int c);
int					ft_isdigit(int c);
int					ft_isascii(int c);
int					ft_isalpha(int c);
int					ft_isalnum(int c);
int					ft_handle_int(int fd, int n);
int					ft_lstsize(t_list *lst);
int					ft_putchar(const char c);
int					ft_atoi(const char *str);
int					ft_check_line(char *str);
int					ft_handle_percent(int fd);
int					ft_putstr(const char *str);
int					ft_putptr(int fd, unsigned long ptr);
int					ft_handle_char(int fd, const char c);
int					ft_printmemory(int fd, unsigned long n);
int					ft_handle_ptr(int fd, unsigned long ptr);
int					ft_printf(const char *format, ...);
int					ft_printf_fd(int fd, const char *format, ...);
int					ft_putnbr_unsigned(int fd, unsigned int n);
int					ft_puthexa(int fd, unsigned int n, int upper);
int					ft_handle_nbr_unsigned(int fd, unsigned int n);
int					ft_handle_str(int fd, const char *str);
int					ft_check_args(int fd, t_format fmt, va_list args);
int					ft_handle_hexa(int fd, unsigned int n, t_format fmt);
int					ft_memcmp(const void *s1, const void *s2, size_t n);
int					ft_strncmp(const char *s1, const char *s2, size_t n);

size_t				ft_strlen(const char *s);
size_t				count_words(const char *s, char c);
size_t				ft_strlcpy(char *dest, const char *src, size_t size);
size_t				ft_strlcat(char *dest, const char *src, size_t size);

char				*ft_itoa(int n);
char				*get_next_line(int fd);
char				*ft_strdup(const char *s);
char				*my_strdup(const char *s);
char				*my_strjoin(char *s1, char *s2);
char				*ft_strchr(const char *s, int c);
char				**ft_split(char const *s, char c);
char				*ft_strrchr(const char *s, int c);
char				*ft_strjoin(char const *s1, char const *s2);
char				*ft_strtrim(char const *s1, char const *set);
char				*ft_strmapi(char const *s, char (*f)(unsigned int, char));
char				*ft_substr(char const *s, unsigned int start, size_t len);
char				*ft_strnstr(const char *big, const char *little,
						size_t len);

void				ft_bzero(void *s, size_t n);
int					ft_putnbr_fd(int n, int fd);
void				ft_putendl_fd(char *s, int fd);
void				free_words(char **words, size_t i);
int					ft_putchar_fd(const char c, int fd);
int					ft_putstr_fd(const char *str, int fd);
void				*ft_calloc(size_t nmemb, size_t size);
void				ft_lstadd_back(t_list **lst, t_list *new);
void				*ft_memchr(const void *s, int c, size_t n);
void				*ft_memset(void *adr, int value, size_t n);
void				ft_lstadd_front(t_list **lst, t_list *new);
void				ft_lstiter(t_list *lst, void (*f)(void *));
void				ft_lstdelone(t_list *lst, void (*del)(void *));
void				ft_lstclear(t_list **lst, void (*del)(void *));
void				parse_format(const char **format, t_format *fmt);
void				*ft_memcpy(void *dest, const void *src, size_t n);
void				*ft_memmove(void *dest, const void *src, size_t n);
void				ft_striteri(char *s, void (*f)(unsigned int, char *));

t_list				*ft_lstlast(t_list *lst);
t_list				*ft_lstnew(void *content);
t_list				*ft_lstmap(t_list *lst, void *(*f)(void *),
						void (*del)(void *));

#endif
