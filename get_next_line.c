#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "get_next_line.h"

static char	*str_merge(char *dest, char *src)
{
	int dest_len;
	int src_len;
	char *new;

	if (dest == NULL)
		return (NULL);

	if (src == NULL)
		return (dest);

	dest_len = strlen(dest);
	src_len = strlen(src);
	new = malloc((dest_len + src_len + 1) * sizeof(char));
	if (new == NULL)
		return (NULL);

	memcpy(new, dest, dest_len);
	memcpy(new + dest_len, src, src_len);
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

	memcpy(str, src, pos);
	str[pos] = '\0';

	return (str);
}

static void	fill_buff(char *buff, char *src, int size, int length)
{
	memcpy(buff, src, size);
	buff[length] = '\0';
}

static char	*case_buff_not_empty_with_line_feed(char *buff, int pos)
{
	char *str;
	size_t buff_len = strlen(buff);

	str = fill_str(buff, pos);
	if (str == NULL)
		return (NULL);

	memmove(buff, buff + pos + 1, buff_len - pos);
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
	size_t buff_len = strlen(buff);
	char *str;
	char *src = stream + pos_stream + 1;
	int size = readchar - pos_stream;

	str = malloc((pos_stream + 1 + buff_len) * sizeof(char));
	if (str == NULL)
		return (NULL);

	memcpy(str, buff, buff_len);
	memcpy(str + buff_len, stream, pos_stream);
	fill_buff(buff, src, size, readchar - (pos_stream + 1));
	str[buff_len + pos_stream] = '\0';

	return(str);
}

static char	*case_stdin_without_line_feed(char *buff, char *stream, int readchar, int fd)
{
	size_t buff_len = strlen(buff);
	char *str_rec;
	char *str;

	str = malloc((buff_len + readchar + 1) * sizeof(char));
	if (str == NULL)
		return (NULL);

	memcpy(str, buff, buff_len);
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
	size_t buff_len = strlen(buff);
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
