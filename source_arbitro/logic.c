
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#include "defaults.h"

CmdArgs command_line_arguments(long *wait_time, long *game_duration, int argc, char **argv)
{
    CmdArgs a = NO_ARGS;
    *wait_time = DEFAULT_WAIT_TIME;
    *game_duration = DEFAULT_GAME_TIME;
    char *endptr;
    errno = 0;

    if (argc > 1)
    {
        *game_duration = strtol(argv[1], &endptr, 10);
        if (errno == ERANGE || *endptr != '\0' || *wait_time < MIN_GAME_TIME)
        {
            *game_duration = DEFAULT_GAME_TIME;
            a = ERROR_GAME_TIME;
        }
        else
        {
            a = MISSING_WAIT_TIME;
        }
    }

    if (argc > 2)
    {
        *wait_time = strtol(argv[2], &endptr, 10);
        if (errno == ERANGE || *endptr != '\0' || *wait_time < MIN_WAIT_TIME)
        {
            *wait_time = DEFAULT_WAIT_TIME;
            if (a == ERROR_GAME_TIME)
                a = ERROR_BOTH;
            else
                a = ERROR_WAIT_TIME;
        }
        else
            a = OK;
    }
    return a;
}

int get_maxplayer()
{
    char *endptr;
    errno = 0;
    long max_players;

    char *max_num_players = getenv(MAXPLAYER_NAME);
    if (max_num_players == NULL)
    {
        max_players = -1;
    }
    else
    {
        max_players = strtol(max_num_players, &endptr, 10);
        if (max_players > MAXPLAYER_DEFAULT || max_players < 2 || errno == ERANGE || *endptr != '\0')
        {
            max_players = -1;
        }
    }
    return max_players;
}

GameDirParsing get_game_dir(char **game_dir)
{
    *game_dir = getenv(DIR_NAME);
    if (*game_dir == NULL)
    {
        *game_dir = GAMEDIR_DEFAULT;
        return ENV_ERROR;
    }
    return ENV_SUCCESS;
}
