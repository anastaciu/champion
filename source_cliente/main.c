#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
}