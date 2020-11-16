#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../global_interface.h"
#include "../global.h"
#include "user_interface.h"

int main()
{
    PlayerLog player;

    char output[OUTPUT_SIZE];
    
    player.player_pid = getpid();

    print(NAME_PROMPT_OUT, STDOUT_FILENO);
    get_user_input(player.name, STDIN_FILENO, MAX_LEN_NAME);
    sprintf(output, USER_NAME_OUT USER_PID_OUT, player.name, player.player_pid);  
    print(output, STDOUT_FILENO);

    char named_pipe[MAX_LEN_NAME];

    sprintf(named_pipe, CLIENT_LOG_FIFO, player.player_pid);

    if (mkfifo(named_pipe, 0777) == -1)
    {
        perror(PIPE_ERROR);
        return EXIT_FAILURE;
    }

    int s_fifo_fd;
    
    if((s_fifo_fd = open(SERVER_LOG_FIFO, O_WRONLY)) == -1){
        sprintf(output, ERROR_FIFO_OPEN, named_pipe);
        perror(output);
    }



}