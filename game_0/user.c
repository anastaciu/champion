#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include <ctype.h>

#include "../global_interface.h"

void print_chars(size_t size, const char *c, int fd_out)
{
    for (size_t i = 0; i < size; i++)
        write(fd_out, c, 2);
    fflush(stdout);
}

void print_word(size_t size, const char *str, int fd_out)
{
    for (size_t i = 0; i < size; i++)
    {
        write(fd_out, &str[i], 1);
        write(fd_out, " ", 1);
    }
    write(fd_out, "\n", 1);
    fflush(stdout);
}

void print_user_interface(const char *word, size_t size, int fd, const char *spacer, const char *menu)
{
    print(spacer, fd);
    print_chars(size * 2 - 1, "*", fd);
    print(spacer, fd);
    print_word(size, word, fd);
    print(spacer, fd);
    print_chars(size * 2 - 1, "*", fd);
    print(menu, fd);
}

void fill_word(char *str, size_t size, const char *c)
{
    size_t i;
    for (i = 0; i < size; i++)
        str[i] = c[0];
    str[i] = '\0';
}