/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 18:04:46 by maximart          #+#    #+#             */
/*   Updated: 2025/03/26 18:20:00 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/libft.h"

int	ft_check_args(int fd, t_format fmt, va_list args)
{
	if (fmt.conversion == 'c')
		return (ft_handle_char(fd, va_arg(args, int)));
	else if (fmt.conversion == 's')
		return (ft_handle_str(fd, va_arg(args, char *)));
	else if (fmt.conversion == 'p')
		return (ft_handle_ptr(fd, va_arg(args, unsigned long)));
	else if (fmt.conversion == 'd' || fmt.conversion == 'i')
		return (ft_handle_int(fd, va_arg(args, int)));
	else if (fmt.conversion == 'u')
		return (ft_handle_nbr_unsigned(fd, va_arg(args, unsigned int)));
	else if (fmt.conversion == 'x' || fmt.conversion == 'X')
		return (ft_handle_hexa(fd, va_arg(args, unsigned int), fmt));
	else if (fmt.conversion == '%')
		return (ft_handle_percent(fd));
	return (0);
}

int	ft_printf_fd(int fd, const char *format, ...)
{
	int			count;
	va_list		args;
	t_format	fmt;

	count = 0;
	va_start(args, format);
	while (*format)
	{
		if (*format == '%' && *(format + 1))
		{
			format++;
			parse_format(&format, &fmt);
			count += ft_check_args(fd, fmt, args);
		}
		else
			count += ft_putchar_fd(*format, fd);
		format++;
	}
	va_end(args);
	return (count);
}

int	ft_printf(const char *format, ...)
{
	int			count;
	va_list		args;
	t_format	fmt;

	count = 0;
	va_start(args, format);
	while (*format)
	{
		if (*format == '%' && *(format + 1))
		{
			format++;
			parse_format(&format, &fmt);
			count += ft_check_args(STDOUT_FILENO, fmt, args);
		}
		else
			count += ft_putchar(*format);
		format++;
	}
	va_end(args);
	return (count);
}
