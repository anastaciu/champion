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