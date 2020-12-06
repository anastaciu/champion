
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "defaults.h"
#include "../global_interface.h"

void *com_thread(void *arg)
{
    MsgThrd *msg_trd = (MsgThrd *)arg;
    PlayerLog plr;
    //PlayerMsg plr_msg;
    while (msg_trd->keep_alive == 1)
    {       
        int r = read(msg_trd->clt_fifo_fd, &plr, sizeof plr);
        if (r == sizeof plr)
        {          
            if (plr.p_msg.log_state == REMOVED)
            {
                close(msg_trd->clt_fifo_fd);
                close(msg_trd->srv_fifo_fd);
                remove(msg_trd->clt_fifo_name);
                print("\nFoi removido pelo árbitro!\n", STDOUT_FILENO);
                
                exit(EXIT_SUCCESS);
            }
            else if (plr.p_msg.log_state == EXITED)
            {
                close(msg_trd->clt_fifo_fd);
                close(msg_trd->srv_fifo_fd);
                remove(msg_trd->clt_fifo_name);
                print("\nO servidor foi encerrado!\n", STDOUT_FILENO);
                
                exit(EXIT_SUCCESS);
            }
            strcpy(msg_trd->msg->game_name, plr.p_msg.game_name);
            strcpy(msg_trd->msg->msg, plr.p_msg.msg);
            msg_trd->msg->points = plr.p_msg.points;
            msg_trd->msg->log_state = plr.p_msg.log_state;
        }
        else
        {
            print("\nDados corrompidos\n", STDIN_FILENO);
            exit(EXIT_FAILURE);
        }
    }
    return NULL;
}
