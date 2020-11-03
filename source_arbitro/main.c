#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "defaults.h"
#include "interface.h"
#include "../global.h"
#include "output_interface.h"

int main(int argc, char *argv[])
{
    char output[OUTPUT_SIZE];

    long wait_time = DEFAULT_WAIT_TIME;
    long game_duration = DEFAULT_GAME_TIME;

    switch (command_line_arguments(&wait_time, &game_duration, argc, argv))
    {
    case NO_ARGS:
        write(FD_OUT, NO_ARGS_OUT, strlen(NO_ARGS_OUT));
        sprintf(output, WAIT_TIME_OUT, wait_time);
        write(FD_OUT, output, sizeof strlen(output));
        sprintf(output, GAME_DURATION_OUT, game_duration);
        write(FD_OUT, output, strlen(output));
        fflush(stdout);
        break;

    case ERROR_GAME_TIME:

    break;
    case ERROR_WAIT_TIME:

    break;
    case ERROR_BOTH:

    break;


    }
}