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

static void *my_memcpy(void *dest, const void *src, size_t n)
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

static void *my_memmove(void *dest, const void *src, size_t n)
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

// 1 buff est vide
// 2 buff n'est pas vide et contient un '\n'
// 3 buff n'est pas vide et ne contient pas '\n'
// 4 stdin contient un '\n' dans READ_SIZE
// 5 stdin ne contient pas de '\n' dans READ_SIZE

// (1+3)(4+5) + 2

int	find_n(char *str)
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

char	*get_next_line(const int fd)
{
	static char buff[READ_SIZE] = "hello\nte\0";
	int pos = find_n(buff);

	// 2 buff n'est pas vide et contient un '\n'
	if (pos != -1)
	{
		printf("111111111111111111\n");
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
		printf("buff_len - (pos + 1) = %d\n", buff_len - (pos + 1));
		return (str);
	}
	else
	{
		// 1 buff est vide
		printf("2222222222222222222222\n");
		if (buff[0] == '\0')
		{
			char stream[READ_SIZE];
			int readchar = read(fd, stream, READ_SIZE);
			printf("readchar %d\n", readchar);
			if (readchar == -1 || readchar == 0)
				return (NULL);
			int pos_stream = find_n(stream);

			// 4 stdin contient un '\n' dans READ_SIZE
			if (pos_stream != -1)
			{
				printf("333333333333333333\n");
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
				printf("44444444444444444444444444\n");
				my_memcpy(buff, stream, READ_SIZE);
				return (get_next_line(fd));
			}
		}

		// 3 buff n'est pas vide et ne contient pas '\n'
		else
		{
			printf("5555555555555555555\n");
			char stream[READ_SIZE];
			int readchar = read(fd, stream, READ_SIZE);
			int buff_len = my_strlen(buff);

			if (readchar == 0)
			{
				printf("777777777777777777\n");
				char *str = malloc((buff_len + 1) * sizeof(char));

				my_memcpy(str, buff, buff_len);
				buff[0] = '\0';

				return (str);
			}
			if (readchar == -1)
				return (NULL);
			int pos_stream = find_n(stream);

			// 4 stdin contient un '\n' dans READ_SIZE
			if (pos_stream != -1)
			{
				printf("888888888888888888888\n");
				if (pos_stream == 0)
				{
					char *str = malloc((buff_len + 1) * sizeof(char));

					my_memcpy(str, buff, buff_len);
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

				printf("readchar - (pos_stream + 1) = %d\n", readchar - (pos_stream + 1));


				return (str);
			}

			// 5 stdin ne contient pas de '\n' dans READ_SIZE
			else
			{
				printf("999999999999999999\n");
				// char *str = malloc((buff_len + READ_SIZE) * sizeof(char));
				// if (str == NULL)
				// 	return (NULL);

				// my_memcpy(str, buff, buff_len);
				// my_memcpy(str + buff_len, get_next_line(fd), READ_SIZE);
				// printf("str = %s\n", str);

				char *str = malloc((buff_len + 1) * sizeof(char));

				my_memcpy(str, buff, buff_len);
				str[buff_len] = '\0';

				printf("RECURSION\n");
				return (str);
			}
		}
	}

	return (NULL);
}


// else
// {
// 	char stream[READ_SIZE];
// 	int readchar = read(fd, stream, READ_SIZE);

// 	return (buff + stream[until '\n'] + '\0');
// 	empty(buff);
// 	buff = stream[after '\n'];
// }
// else
// {
// 	char *stream = automallocmagic();

// 	while (find_n(stream))
// 	{
// 		int readchar = read(fd, stream, READ_SIZE);
// 	}

// 	return (buff + stream[until '\n'] + '\0');
// 	empty(buff);
// 	buff = stream[after '\n'];
// }
