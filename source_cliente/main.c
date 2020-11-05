#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../global_interface.h"
#include "../global.h"
#include "user_interface.h"

int main()
{
    PlayerData player;

    char output[OUTPUTSIZE];
    
    print(NAME_PROMPT_OUT, FD_OUT);
    get_user_input(player.name, FD_IN, MAX_LEN_NAME);
    sprintf(output, USER_NAME_OUT, player.name);  
    print(output, FD_OUT);
}