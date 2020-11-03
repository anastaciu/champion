#include <stdio.h>
#include <string.h>

void print(const char *str, int fd_out)
{
    write(fd_out, str, strlen(str));
    fflush(stdout);
}
