#ifndef _GET_NEXT_LINE_H_
#define _GET_NEXT_LINE_H_

#ifndef READ_SIZE
#define READ_SIZE 20

#endif

char	*str_merge(char const *dest, char const *src, int size);
char	*get_str_end_file(char *content, int count);
char	*get_next_line(const int fd);

#endif // _GET_NEXT_LINE_H_