/* ************************************************************************** */
/*                                                                            */
/*                                                       :::      ::::::::    */
/*   get_next_line.c                                   :+:      :+:    :+:    */
/*                                                   +:+ +:+         +:+      */
/*   By: spuschma <spuschma@student.42vienna.com>  #+#  +:+       +#+         */
/*                                               +#+#+#+#+#+   +#+            */
/*   Created: 2026/10/07 13:22:21 by spuschma         #+#    #+#              */
/*   Updated: 2026/10/09 15:11:58 by spuschma        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */
#include <__stdarg_va_list.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct s_line
{
	char	*str;
	size_t	end;
	size_t	size;
	size_t	newline;
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

	size = line.end - line.newline;
	if (size == 0)
	{
		ret = malloc(1);
		*ret = 0;
		return (ret);
	}
	ret = malloc(BUFFER_SIZE);
	i = -1;
	j = line.newline + 1;
	while (++i < size - 1)
		ret[i] = line.str[j++];
	while (i < size)
		ret[i++] = 0;
	return (ret);
}

int	init_line(char *buf, t_line *line)
{
	line->size = BUFFER_SIZE;
	line->str = buf;
	if (!line->str)
		line->str = malloc(line->size);
	if (!line->str)
		return (0);
	line->end = -1;
	while (line->str[++line->end])
		;
	return (1);
}

char	*get_next_line(int fd)
{
	static char	*buffers[1024];
	t_line		line;
	ssize_t		read_result;

	if (!init_line(buffers[fd], &line))
		return (NULL);
	while (1)
	{
		if (line.end + BUFFER_SIZE > line.size)
			if (!double_line_size(&line))
				return (NULL);
		read_result = read(fd, line.str + line.end, BUFFER_SIZE);
		if (!read_result)
		{
			if (!line.newline)
				return (NULL);
			line.str[line.end] = '\n';
		}
		while (read_result--)
		{
			if (line.str[line.end++] != '\n')
				continue ;
			line.newline = line.end - 1;
			line.end += read_result;
			buffers[fd] = get_leftover(line);
			line.str[line.newline + 1] = '\0';
			if (!buffers[fd])
				return (NULL);
			return (line.str);
		}
	}
}
