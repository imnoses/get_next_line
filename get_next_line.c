/* ************************************************************************** */
/*                                                                            */
/*                                                       :::      ::::::::    */
/*   get_next_line.c                                   :+:      :+:    :+:    */
/*                                                   +:+ +:+         +:+      */
/*   By: spuschma <spuschma@student.42vienna.com>  #+#  +:+       +#+         */
/*                                               +#+#+#+#+#+   +#+            */
/*   Created: 2026/10/07 13:22:21 by spuschma         #+#    #+#              */
/*   Updated: 2026/10/09 18:09:36 by spuschma        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */
#include <__stdarg_va_list.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct s_line
{
	char	*str;
	size_t	end;
	size_t	size;
	ssize_t	nl;
}			t_line;

size_t		BUFFER_SIZE = 2048;

int	double_line_size(t_line *line)
{
	char	*new;
	size_t	i;

	line->size *= 2;
	new = malloc(line->size);
	if (!new)
		return (0);
	i = -1;
	while (++i < line->end)
		new[i] = line->str[i];
	free(line->str);
	line->str = new;
	return (1);
}

char	*get_leftover(t_line line)
{
	size_t	size;
	char	*ret;
	size_t	i;
	size_t	j;

	ret = malloc(BUFFER_SIZE);
	size = line.end - line.nl;
	i = -1;
	j = line.nl + 1;
	while (++i < size - 1)
		ret[i] = line.str[j++];
	while (i < size)
		ret[i++] = 0;
	return (ret);
}

int	init_line(char *buf, t_line *l)
{
	l->size = BUFFER_SIZE;
	l->str = buf;
	if (!l->str)
	{
		l->str = malloc(l->size);
		if (!l->str)
			return (0);
		l->str[0] = '\0';
	}
	l->end = -1;
	l->nl = -1;
	while (l->str[++l->end])
		if (l->nl < 0 && l->str[l->end] == '\n')
			l->nl = l->end;
	return (1);
}

int	read_buf(t_line *l, int fd)
{
	ssize_t	read_result;
	size_t	i;

	read_result = read(fd, l->str + l->end, BUFFER_SIZE);
	i = l->end;
	l->end += read_result;
	while (i < l->end)
	{
		if (l->str[i] == '\n')
		{
			l->nl = i;
			break ;
		}
		i++;
	}
	return (read_result);
}

char	*get_next_line(int fd)
{
	static char	*bufs[1024];
	t_line		l;

	if (fd < 0 || fd > 1024)
		return (NULL);
	if (!init_line(bufs[fd], &l))
		return (NULL);
	bufs[fd] = NULL;
	while (l.nl < 0)
	{
		if (l.end + BUFFER_SIZE >= l.size)
			if (!double_line_size(&l))
				return (NULL);
		if (!read_buf(&l, fd))
			break ;
	}
	if (l.nl < 0)
	{
		if (l.end > 0)
			return (l.str[l.end] = 0, free(bufs[fd]), bufs[fd] = NULL, l.str);
		return (free(bufs[fd]), bufs[fd] = NULL, free(l.str), NULL);
	}
	bufs[fd] = get_leftover(l);
	if (!bufs[fd])
		return (free(l.str), NULL);
	return (l.str[l.nl + 1] = 0, l.str);
}
