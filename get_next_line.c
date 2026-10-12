/* ************************************************************************** */
/*                                                                            */
/*                                                       :::      ::::::::    */
/*   get_next_line.c                                   :+:      :+:    :+:    */
/*                                                   +:+ +:+         +:+      */
/*   By: spuschma <spuschma@student.42vienna.com>  #+#  +:+       +#+         */
/*                                               +#+#+#+#+#+   +#+            */
/*   Created: 2026/10/07 13:22:21 by spuschma         #+#    #+#              */
/*   Updated: 2026/10/12 12:30:17 by spuschma        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */
#include "get_next_line.h"
#include <stdlib.h>

static int	double_line_size(t_line *line)
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

static char	*get_leftover(t_line line)
{
	size_t	size;
	char	*ret;
	size_t	i;
	size_t	j;

	ret = malloc(BUFFER_SIZE);
	if (!ret)
		return (NULL);
	size = line.end - line.nl;
	i = -1;
	j = line.nl + 1;
	while (++i < size - 1)
		ret[i] = line.str[j++];
	while (i < size)
		ret[i++] = 0;
	return (ret);
}

static int	init_line(char *buf, t_line *l)
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

// on read error we don't want to print the line so we set l.end to 0
static int	read_buf(t_line *l, int fd)
{
	ssize_t	read_result;
	size_t	i;

	read_result = read(fd, l->str + l->end, BUFFER_SIZE);
	if (read_result < 0)
		return (l->end = 0, read_result);
	if (read_result == 0)
		return (read_result);
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

	if (fd < 0 || fd >= 1024 || !init_line(bufs[fd], &l))
		return (NULL);
	bufs[fd] = NULL;
	while (l.nl < 0)
	{
		if (l.end + BUFFER_SIZE >= l.size)
			if (!double_line_size(&l))
				return (free(l.str), NULL);
		if (read_buf(&l, fd) <= 0)
			break ;
	}
	if (l.nl < 0 && l.end > 0)
		return (l.str[l.end] = 0, l.str);
	if (l.nl < 0)
		return (free(l.str), NULL);
	bufs[fd] = get_leftover(l);
	if (!bufs[fd])
		return (free(l.str), NULL);
	return (l.str[l.nl + 1] = 0, l.str);
}

/*
#include <fcntl.h>
#include <stdio.h>

int	main(int argc, char **argv)
{
	int		fd;
	char	*line;

	if (argc < 2)
	{
		printf("No file provided, using STDIN!\n");
		fd = 0;
	}
	else
		fd = open(argv[1], O_RDONLY);
	printf("Every time you press Enter you get a line of the file\n\n");
	line = malloc(1);
	while (line)
	{
		free(line);
		line = get_next_line(fd);
		if (fd)
		{
			printf("\033[A");
			getchar();
		}
		printf("%s", line);
	}
}
*/
