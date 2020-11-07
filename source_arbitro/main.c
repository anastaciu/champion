#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include "defaults.h"
#include "logic_interface.h"
#include "../global_interface.h"
#include "../global.h"
#include "user_interface.h"

int main(int argc, char **argv)
{
    char output[OUTPUT_SIZE];

    ServerSettings server;
 
    switch (command_line_arguments(&server.wait_time, &server.game_duration, argc, argv))
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
    case OK:
        break;
    }
    sprintf(output, WAIT_TIME_OUT, server.wait_time);
    print(output, FD_OUT);
    sprintf(output, GAME_DURATION_OUT, server.game_duration);
    print(output, FD_OUT);

    if (get_game_dir(&server.game_dir) == ENV_ERROR)
    {
        sprintf(output, GAMEDIR_ERROR_OUT, server.game_dir);
        print(output, FD_OUT);
    }
    else{
        sprintf(output, PARSED_GAMEDIR_OUT, server.game_dir);
        print(output, FD_OUT);
    }

    server.n_players = get_maxplayer();

    if(server.n_players == ENV_ERROR){
        server.n_players = MAXPLAYER_DEFAULT;
        sprintf(output, MAXPLAYER_ERROR_OUT, MAXPLAYER_DEFAULT);
        print(output, FD_OUT);
    }
    else{       
        sprintf(output, PARSED_MAXPLAYER_OUT, server.n_players);
        print(output, FD_OUT);
    }

}