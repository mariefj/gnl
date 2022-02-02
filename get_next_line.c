#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include "get_next_line.h"

static size_t	my_strlen(char *str)
{
	int i = 0;

	if (str == NULL)
		return (0);

	while (str[i] != '\0')
	{
		i++;
	}

	return (i);
}

static void	*my_memcpy(void *dest, const void *src, size_t n)
{
	char *dest_str;
	const char *src_str;
	long *dest_long = dest;
	const long *src_long = src;

	while (n >= sizeof(long) * 4)
	{
		n -= sizeof(long) * 4;
		*dest_long++ = *src_long++;
		*dest_long++ = *src_long++;
		*dest_long++ = *src_long++;
		*dest_long++ = *src_long++;
	}
	while (n >= sizeof(long))
	{
		n -= sizeof(long);
		*dest_long++ = *src_long++;
	}
	dest_str = (char *)dest_long;
	src_str = (const char *)src_long;
	while (n--)
	{
		*dest_str++ = *src_str++;
	}

	return (dest);
}

static void	*my_memmove(void *dest, const void *src, size_t n)
{
	char *dest_str = (char *)dest;
	char *src_str = (char *)src;
	long *dest_long = (long *)dest;
	long *src_long = (long *)src;

	if (dest > src)
	{
		while (n % sizeof(long) != 0)
		{
			n--;
			dest_str[n] = src_str[n];
		}
		for (size_t i = n / sizeof(long); i > 0; i--)
		{
			dest_long[i - 1] = src_long[i - 1];
		}
	}
	else
		my_memcpy(dest, src, n);

	return (dest);
}

static char	*str_merge(char *dest, char *src)
{
	int dest_len = my_strlen(dest);
	int src_len = my_strlen(src);
	char *new;

	new = malloc((dest_len + src_len + 1) * sizeof(char));
	if (new == NULL)
		return (NULL);

	my_memcpy(new, dest, dest_len);
	my_memcpy(new + dest_len, src, src_len);

	new[dest_len + src_len] = '\0';
	free(dest);

	return (new);
}

static int	find_n(char *str)
{
	int i = 0;

	while (str[i] != '\0')
	{
		if (str[i] == '\n')
			return (i);

		i++;
	}

	return (-1);
}

static char	*fill_str(char *src, int pos)
{
	char *str;

	str = malloc((pos + 1) * sizeof(char));
	if (str == NULL)
		return (NULL);

	my_memcpy(str, src, pos);
	str[pos] = '\0';

	return (str);
}

static void	fill_buff(char *buff, char *src, int size, int length)
{
	my_memcpy(buff, src, size);
	buff[length] = '\0';
}

static char	*case_buff_not_empty_with_line_feed(char *buff, int pos)
{
	char *str;
	size_t buff_len = my_strlen(buff);

	str = fill_str(buff, pos);
	if (str == NULL)
		return (NULL);

	my_memmove(buff, buff + pos + 1, buff_len - pos);
	buff[buff_len - (pos + 1)] = '\0';

	return (str);
}

static char	*fill_spe_without_line_feed(char *buff, char *stream, int pos_stream, int readchar)
{
	char *str;
	char *src = stream + pos_stream + 1;
	int size = readchar - pos_stream;
	int length = readchar - (pos_stream + 1);

	str = fill_str(stream, pos_stream);
	if (str == NULL)
		return (NULL);

	fill_buff(buff, src, size, length);

	return (str);
}

static char	*case_buff_empty(char *buff, int fd)
{
	char stream[READ_SIZE + 1];
	int readchar;
	int pos_stream;

	readchar = read(fd, stream, READ_SIZE);
	if (readchar == -1 || readchar == 0)
		return (NULL);

	stream[readchar] = '\0';
	pos_stream = find_n(stream);
	if (pos_stream != -1)
		return (fill_spe_without_line_feed(buff, stream, pos_stream, readchar));
	else {
		if (readchar != READ_SIZE)
			return (fill_str(stream, readchar));

		fill_buff(buff, stream, readchar, readchar);

		return (get_next_line(fd));
	}
}

static char	*case_stdin_with_line_feed(char *buff, char *stream, int pos_stream, int readchar)
{
	size_t buff_len = my_strlen(buff);
	char *str;
	char *src = stream + pos_stream + 1;
	int size = readchar - pos_stream;

	str = malloc((pos_stream + 1 + buff_len) * sizeof(char));
	if (str == NULL)
		return (NULL);

	my_memcpy(str, buff, buff_len);
	my_memcpy(str + buff_len, stream, pos_stream);
	fill_buff(buff, src, size, readchar - (pos_stream + 1));
	str[buff_len + pos_stream] = '\0';

	return(str);
}

static char	*case_stdin_without_line_feed(char *buff, char *stream, int readchar, int fd)
{
	size_t buff_len = my_strlen(buff);
	char *str_rec;
	char *str;

	str = malloc((buff_len + readchar + 1) * sizeof(char));
	if (str == NULL)
		return (NULL);

	my_memcpy(str, buff, buff_len);
	str[buff_len] = '\0';

	buff[0] = '\0';

	str = str_merge(str, stream);
	if (readchar != READ_SIZE)
		return (str);

	str_rec = get_next_line(fd);
	str = str_merge(str, str_rec);
	free(str_rec);

	return (str);
}

static char	*manage_cases_readchar(char *buff, size_t buff_len, int readchar)
{
	char *str;

	if (readchar == 0)
	{
		str = fill_str(buff, buff_len);
		buff[0] = '\0';

		return (str);
	}
	if (readchar == -1)
		return (NULL);

	return (NULL);
}

static char	*case_buff_not_empty(char *buff, int fd)
{
	char stream[READ_SIZE + 1];
	int readchar;
	size_t buff_len = my_strlen(buff);
	char *str;

	readchar = read(fd, stream, READ_SIZE);
	if (readchar == 0 || readchar == -1)
		return (manage_cases_readchar(buff, buff_len, readchar));

	stream[readchar] = '\0';

	if (find_n(stream) != -1)
		str = case_stdin_with_line_feed(buff, stream, find_n(stream), readchar);
	else
		str = case_stdin_without_line_feed(buff, stream, readchar, fd);
	return (str);
}

char	*get_next_line(const int fd)
{
	static char buff[READ_SIZE + 1] = "\0";
	int pos = find_n(buff);
	char *str;

	if (pos != -1)
		str = case_buff_not_empty_with_line_feed(buff, pos);
	else if (buff[0] == '\0')
	{
		str =  case_buff_empty(buff, fd);
	}
	else
		str = case_buff_not_empty(buff, fd);

	return(str);
}
