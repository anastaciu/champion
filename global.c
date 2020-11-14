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

void _to_upper(char *str){
    while(*str){
        *str = toupper(*str);
        str++;
    }
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
    char *begin;
    char *end;
    size_t input_length;
    size_t read_chars;
    do
    {
        read_chars = read(fd, input, size);
        begin = input;
        end = &input[read_chars - 1];

        *(end--) = '\0'; 

        while(end >= begin && isspace(*end)){                  
            end--;
        }
        *(end + 1) = '\0';

        while(begin < end && isspace(*begin)){
            begin++;
        }

        input_length = strlen(begin); 

    } while (input_length < 1);

    memcpy(input, begin, input_length + 1);
    _to_upper(input);
    _clear_buffer(fd);

    return input_length;
}