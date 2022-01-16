#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

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

static char	*my_append_c(char *str, char c)
{
	int i = 0;
	int len = my_strlen(str) + 2;
	char *new_str = (char *)malloc(len * sizeof(char));

	if (new_str == NULL)
		return (NULL);
	if (str == NULL)
	{
		str = (char *)malloc(1 * sizeof(char));
		if (str == NULL)
			return (NULL);
		str[0] = '\0';
	}
	while (str[i] != '\0')
	{
		new_str[i] = str[i];
		i++;
	}
	new_str[i] = c;
	new_str[i + 1] = '\0';
	free(str);

	return (new_str);
}

static char	*str_merge(char const *dest, char const *src, int size)
{
	int i = 0;
	int j = 0;
	int len = (dest == NULL ? 0 : my_strlen(dest)) + (src == NULL ? 0 : size);
	char *new = (char *)malloc((len + 1) * sizeof(char));

	while (dest != NULL && dest[i] != '\0')
	{
		new[i] = dest[i];
		i++;
	}
	while (src != NULL && j < size)
	{
		new[i] = src[j];
		i++;
		j++;
	}
	new[i] = '\0';

	return (new);
}

static char	*get_str_end_file(char *content, int count)
{
	int i = 0;
	char *str = (char *)malloc((count + 1) * sizeof(char));

	while (i < count)
	{
		str[i] = content[i];
		i++;
	}
	str[i] = '\0';

	return (str);
}

static char	*get_content_to_str(int fd)
{
	int count = READ_SIZE;
	char content[READ_SIZE];
	char *str = NULL;

	while (count != 0)
	{
		count = read(fd, content, READ_SIZE);
		if (count == -1)
			return (NULL);

		if (count == READ_SIZE)
			str = str_merge(str, content, count);
		else
			str = str_merge(str, get_str_end_file(content, count), count);
	}

	return (str);
}

char	*get_next_line(const int fd)
{
	static char *str = NULL;
	int size_lign = 0;
	char *lign = NULL;

	if (str == NULL)
	{
		str = get_content_to_str(fd);
	}
	while (str[size_lign] != '\n' && str[size_lign] != '\0')
	{
		lign = my_append_c(lign, str[size_lign]);
		size_lign++;
	}
	str += (size_lign + 1);

	return (lign);
}
