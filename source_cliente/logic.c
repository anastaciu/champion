
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>

#include "defaults.h"
#include "../utils_interface.h"

void *com_thread(void *arg)
{
    MsgThrd *msg_trd = (MsgThrd *)arg;
    PlayerLog plr;
    while (msg_trd->keep_alive == 1)
    {
        int r = read(msg_trd->clt_fifo_fd, &plr, sizeof plr);
        if (r == sizeof plr)
        {
            if (plr.p_msg.log_state == REMOVED)
            {
                close(msg_trd->clt_fifo_fd);
                close(msg_trd->srv_fifo_fd);
                remove(msg_trd->p_log->player_fifo);
                print("\nFoi removido pelo árbitro! (Processo termina dentro de 5 segundos)\n", STDOUT_FILENO);
                msg_trd->keep_alive = 0;
            }
            else if (plr.p_msg.log_state == EXITED)
            {
                close(msg_trd->clt_fifo_fd);
                close(msg_trd->srv_fifo_fd);
                remove(msg_trd->p_log->player_fifo);
                print("\nO servidor foi encerrado! (Processo termina dentro de 5 segundos)\n", STDOUT_FILENO);
                msg_trd->keep_alive = 0;
            }
            else if (plr.p_msg.log_state == QUITED)
            {
                close(msg_trd->clt_fifo_fd);
                close(msg_trd->srv_fifo_fd);
                remove(msg_trd->p_log->player_fifo);
                print("\nSaiu do jogo! (Processo termina dentro de 5 segundos)\n", STDOUT_FILENO);
                msg_trd->keep_alive = 0;
                pthread_kill(msg_trd->init_tid, SIGUSR1);
            }
            else
            {
                print("\nMensagem do servidor: ", STDOUT_FILENO);
                print(plr.p_msg.msg, STDOUT_FILENO);
                print("\n>", STDOUT_FILENO);
            }

            strcpy(msg_trd->p_log->p_msg.game_name, plr.p_msg.game_name);
            strcpy(msg_trd->p_log->p_msg.msg, plr.p_msg.msg);
            msg_trd->p_log->p_msg.points = plr.p_msg.points;
            msg_trd->p_log->p_msg.log_state = plr.p_msg.log_state;
        }
        else
        {
            print("\nDados corrompidos\n", STDIN_FILENO);
            msg_trd->keep_alive = 0;
        }
    }
    return NULL;
}

void sig_handler(int sig)
{
    if (sig == SIGUSR1)
    {
        sleep(5);
        exit(EXIT_SUCCESS);
    }
}