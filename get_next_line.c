#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include "get_next_line.h"

static int	my_strlen(char const *str)
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
	char *new = malloc((dest_len + src_len + 1) * sizeof(char));

	if (new == NULL)
		return (NULL);

	my_memcpy(new, dest, dest_len);
	my_memcpy(new + dest_len, src, src_len);

	new[dest_len + src_len + 1] = '\0';
	free(dest);

	return (new);
}

int	find_n(char *str)
{
	int i = 0;

	while (str[i] != '\0' && i < READ_SIZE)
	{
		if (str[i] == '\n')
			return (i);
		i++;
	}

	return (-1);
}

char	*get_next_line(const int fd)
{
	static char buff[READ_SIZE] = "\0";
	int pos = find_n(buff);

	// 2 buff n'est pas vide et contient un '\n'
	if (pos != -1)
	{
		if (pos == 0)
			return (buff);
		char *str = malloc((pos + 1) * sizeof(char));
		if (str == NULL)
			return (NULL);
		int buff_len = my_strlen(buff);

		my_memcpy(str, buff, pos);
		str[pos] = '\0';
		my_memmove(buff, buff + pos + 1, buff_len - pos);
		buff[buff_len - (pos + 1)] = '\0';
		return (str);
	}
	else
	{
		// 1 buff est vide
		if (buff[0] == '\0')
		{
			char stream[READ_SIZE];
			int readchar = read(fd, stream, READ_SIZE);
			if (readchar == -1 || readchar == 0)
				return (NULL);
			stream[readchar] = '\0';
			int pos_stream = find_n(stream);

			// 4 stdin contient un '\n' dans READ_SIZE
			if (pos_stream != -1)
			{
				char *str = malloc((pos_stream + 1) * sizeof(char));
				if (str == NULL)
					return (NULL);

				my_memcpy(str, stream, pos_stream);
				str[pos_stream] = '\0';

				my_memcpy(buff, stream + pos_stream + 1, readchar - pos_stream);
				buff[readchar - (pos_stream + 1)] = '\0';

				return (str);
			}

			// 5 stdin ne contient pas de '\n' dans READ_SIZE
			else
			{
				my_memcpy(buff, stream, readchar);
				buff[readchar] = '\0';
				return (get_next_line(fd));
			}
		}

		// 3 buff n'est pas vide et ne contient pas '\n'
		else
		{
			char stream[READ_SIZE];
			int readchar = read(fd, stream, READ_SIZE);
			int buff_len = my_strlen(buff);

			if (readchar == 0)
			{
				char *str = malloc((buff_len + 1) * sizeof(char));

				my_memcpy(str, buff, buff_len);
				str[buff_len] = '\0';
				buff[0] = '\0';

				return (str);
			}
			if (readchar == -1)
				return (NULL);
			stream[readchar] = '\0';
			int pos_stream = find_n(stream);

			// 4 stdin contient un '\n' dans READ_SIZE
			if (pos_stream != -1)
			{
				if (pos_stream == 0)
				{
					char *str = malloc((buff_len + 1) * sizeof(char));

					my_memcpy(str, buff, buff_len);
					str[buff_len] = '\0';
					buff[0] = '\0';

					return (str);
				}
				char *str = malloc(((pos_stream + 1) + buff_len) * sizeof(char));
				if (str == NULL)
					return (NULL);

				my_memcpy(str, buff, buff_len);
				my_memcpy(str + buff_len, stream, pos_stream);
				my_memcpy(buff, stream + pos_stream + 1, readchar - pos_stream);
				buff[readchar - (pos_stream + 1)] = '\0';
				str[buff_len + pos_stream] = '\0';

				return (str);
			}

			// 5 stdin ne contient pas de '\n' dans READ_SIZE
			else
			{
				char *str = malloc((buff_len + readchar + 1) * sizeof(char));

				my_memcpy(str, buff, buff_len);
				str[buff_len] = '\0';

				return (str_merge(str, get_next_line(fd)));
			}
		}
	}

	return (NULL);
}
