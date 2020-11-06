#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "defaults.h"
#include "logic_interface.h"
#include "../global_interface.h"
#include "../global.h"
#include "user_interface.h"

int main(int argc, char **argv)
{
    char output[OUTPUT_SIZE];

    long wait_time = DEFAULT_WAIT_TIME;
    long game_duration = DEFAULT_GAME_TIME;
    int max_players;
    char *game_dir;

    switch (command_line_arguments(&wait_time, &game_duration, argc, argv))
    {
    case NO_ARGS:
        print(NO_ARGS_OUT, FD_OUT);
        print(DEFAULT_OUT, FD_OUT);
        break;
    case ERROR_GAME_TIME:
        print(ERROR_GAME_TIME_OUT, FD_OUT);
        print(DEFAULT_OUT, FD_OUT);
        break;
    case ERROR_WAIT_TIME:
        print(ERROR_WAIT_TIME_OUT, FD_OUT);
        print(DEFAULT_OUT, FD_OUT);
        break;
    case ERROR_BOTH:
        print(ERROR_BOTH_OUT, FD_OUT);
        print(DEFAULT_OUT, FD_OUT);
        break;
    case MISSING_WAIT_TIME:
        print(MISSING_WAIT_TIME_OUT, FD_OUT);
        print(DEFAULT_OUT, FD_OUT);
        break;
    case OK:
        break;
    }
    sprintf(output, WAIT_TIME_OUT, wait_time);
    print(output, FD_OUT);
    sprintf(output, GAME_DURATION_OUT, game_duration);
    print(output, FD_OUT);

    game_dir = getenv(DIR_NAME);
    if (game_dir == NULL)
    {
        game_dir = GAMEDIR_DEFAULT;
        sprintf(output, GAMEDIR_ERROR_OUT, game_dir);
        print(output, FD_OUT);
    }
    else{
        sprintf(output, PARSED_GAMEDIR_OUT, game_dir);
        print(output, FD_OUT);
    }

    max_players = get_maxplayer();

    if(max_players == ENVERROR){
        max_players = MAXPLAYER_DEFAULT;
        sprintf(output, MAXPLAYER_ERROR_OUT, MAXPLAYER_DEFAULT);
        print(output, FD_OUT);
    }
    else{       
        sprintf(output, PARSED_MAXPLAYER_OUT, max_players);
        print(output, FD_OUT);
    }

}