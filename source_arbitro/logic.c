
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "defaults.h"

CmdArgs _parse_arguments(char *opt, char *argv1, char *argv2, CmdArgs flag, long *value, int def, int min)
{
    char *endptr;
    errno = 0;
    CmdArgs a = OK;

    if (strcmp(argv1, opt) == 0)
    {
        *value = strtol(argv2, &endptr, 10);
        if (errno == ERANGE || *endptr != '\0' || *value < min)
        {
            *value = def;
            a = flag;
        }
    }
    else
    {
        a = flag;
    }
    return a;
}

CmdArgs command_line_arguments(long *wait_time, long *game_duration, int argc, char **argv)
{
    CmdArgs a = NO_ARGS, a1, a2, a3, a4;
    *wait_time = DEFAULT_WAIT_TIME;
    *game_duration = DEFAULT_GAME_TIME;

    if (argc > 2)
    {
        a1 = _parse_arguments("-g", argv[1], argv[2], ERROR_GAME_TIME, game_duration, DEFAULT_GAME_TIME, MIN_GAME_TIME);
        a2 = _parse_arguments("-w", argv[1], argv[2], ERROR_WAIT_TIME, wait_time, DEFAULT_WAIT_TIME, MIN_WAIT_TIME);

        if (argc > 4)
        {
            a3 = _parse_arguments("-g", argv[3], argv[4], ERROR_GAME_TIME, game_duration, DEFAULT_GAME_TIME, MIN_GAME_TIME);
            a4 = _parse_arguments("-w", argv[3], argv[4], ERROR_WAIT_TIME, wait_time, DEFAULT_WAIT_TIME, MIN_WAIT_TIME);

            if ((a1 == OK && a4 == OK) || (a2 == OK && a3 == OK))
            {
                return OK;
            }
            else if ((a1 == ERROR_GAME_TIME && a4 == ERROR_WAIT_TIME) && (a2 == ERROR_WAIT_TIME && a3 == ERROR_GAME_TIME))
            {
                return ERROR_BOTH;
            }
            else if ((a1 == ERROR_GAME_TIME && a3 == ERROR_GAME_TIME))
            {
                return ERROR_GAME_TIME;
            }
            else if ((a2 == ERROR_WAIT_TIME && a4 == ERROR_WAIT_TIME))
            {
                return ERROR_WAIT_TIME;
            }
        }
        else if( a1 == OK){
            return ERROR_WAIT_TIME;
        }
        else if (a2 == OK)      
        {
            return ERROR_GAME_TIME;
        }
        else
        {
            return ERROR_BOTH;
        }
              
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
