
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include "defaults.h"

Args command_line_arguments(long *wait_time, long *game_duration, int argc, char **argv)
{
    Args a = NO_ARGS;
    char *endptr;
    errno = 0;
    if (argc > 1)
    {
        *game_duration = strtol(argv[1], &endptr, 10);
        if (errno == ERANGE || *endptr != '\0')
        {
            *game_duration = DEFAULT_GAME_TIME;
            a = ERROR_GAME_TIME;
        }
    }

    if (argc > 2)
    {
        *wait_time = strtol(argv[2], &endptr, 10);
        if (errno == ERANGE || *endptr != '\0')
        {
            *wait_time = DEFAULT_WAIT_TIME;
            if(a == ERROR_GAME_TIME)
                a = ERROR_BOTH;
            else
                a = ERROR_WAIT_TIME;
        }
    }
    return a;
 
    /* 



    *wait_time = DEFAULT_WAIT_TIME;

    if (argc < 2)
    {
        *game_duration = DEFAULT_GAME_TIME;
        *wait_time = DEFAULT_WAIT_TIME;
        return NO_ARGS;
    }

    if (argc < 3)
    {
        char *endptr;
        errno = 0;
        *game_duration = strtol(argv[2], &endptr, 10);
        if (errno == ERANGE || *endptr != '\0')
        {
            *game_duration = DEFAULT_GAME_TIME;
            *wait_time = DEFAULT_WAIT_TIME;
            return ERROR_GAME_TIME;
        }
        *wait_time = DEFAULT_WAIT_TIME;
        return GAME_TIME_ARG;
    }

    else
    {
    } */
}

