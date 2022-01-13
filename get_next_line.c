#include <unistd.h>
#include <stdlib.h>

#include "my.h"
#include "get_next_line.h"

char	*str_merge(char const *dest, char const *src, int size)
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

char	*get_str_end_file(char *content, int count)
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

char	*my_strdup_lign(char const *str, int size)
{
	int i = 0;
	char *strcpy = (char *)malloc((size + 1) * sizeof(char));

	if (strcpy == NULL)
		return (NULL);
	while (str[i] != '\n' || str[i] != '\0')
	{
		strcpy[i] = str[i];
		i++;
	}
	strcpy[i] = '\0';

	return (strcpy);
}

char	*get_file_to_str(int fd)
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
	char *lign;

	if (str == NULL)
	{
		str = get_file_to_str(fd);
	}
	while (*str != '\n' || *str != '\0')
	{
		size_lign++;
	}
	if (str[size_lign] == '\n')
	{
		lign = (char *)malloc((size_lign + 1) * sizeof(char));
		lign = my_strdup_lign(str, size_lign);
		str += (size_lign + 1);
	}
	if (str[size_lign] == '\0')
	{
		return (str);
	}

	return (lign);
}