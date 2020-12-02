
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "defaults.h"
#include "../global_interface.h"

void *com_thread(void *arg)
{

    MsgThrd *msg_trd = (MsgThrd *)arg;
    PlayerMsg plr_msg;
    int r = read(msg_trd->clt_fifo_fd, &plr_msg, sizeof plr_msg);
    if (r == sizeof plr_msg)
    {
        msg_trd->msg = &plr_msg;
        if (plr_msg.log_state == REMOVED)
        {           
            close(msg_trd->clt_fifo_fd);
            close(msg_trd->srv_fifo_fd);
            remove(msg_trd->clt_fifo_name);
            print("Foi removido pelo árbitro!\n", STDOUT_FILENO);
            exit(EXIT_SUCCESS);
        }
        else if(plr_msg.log_state == EXITED){
            close(msg_trd->clt_fifo_fd);
            close(msg_trd->srv_fifo_fd);
            remove(msg_trd->clt_fifo_name);
            print("O servidor foi encerrado!\n", STDOUT_FILENO);
            exit(EXIT_SUCCESS);
        }
    }
    else{
        print("Dados corrompidos\n", STDIN_FILENO);
        exit(EXIT_FAILURE);
    }

    return NULL;
}
