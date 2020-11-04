#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include <ctype.h>

void print(const char *str, int fd_out)
{
    write(fd_out, str, strlen(str));
    fflush(stdout);
}

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

void _clear_buffer(int fd)
{
    struct pollfd pfd = {.fd = fd, .events = POLLIN};
    if (poll(&pfd, 1, 0) > 0)
    {
        if (pfd.revents & POLLIN)
        {
            char c;
            while (read(fd, &c, 1) > 0 && c != '\n')
            {
                continue;
            }
        }
    }
}

void fill_word(char *str, size_t size, const char *c)
{
    size_t i;
    for (i = 0; i < size; i++)
        str[i] = c[0];
    str[i] = '\0';
}

size_t get_user_input(char *input, int fd, int size)
{
    size_t read_chars;
    do
    {
        read_chars = read(fd, input, size);
        input[strcspn(input, "\t\n ")] = '\0';

    } while (strlen(input) < 1);

    input[read_chars - 1] = '\0';
    size_t length = strcspn(input, "\t\n ");
    input[length] = '\0';

    for (size_t i = 0; i < strlen(input); i++)
    {
        input[i] = toupper(input[i]);
    }
    _clear_buffer(fd);
    return length;
}
