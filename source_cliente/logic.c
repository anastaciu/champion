
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "defaults.h"
#include "../utils_interface.h"
#include "ui_constants.h"

void *com_thread(void *arg)
{
    MsgThrd *msg_trd = (MsgThrd *)arg;

    do
    {
        int r = read(msg_trd->clt_fifo_fd, msg_trd->msg, sizeof(ComMsg));
        if (r == sizeof(ComMsg))
        {
            if (msg_trd->msg->log_state == REMOVED)
            {
                close(msg_trd->clt_fifo_fd);
                close(msg_trd->srv_fifo_fd);
                remove(msg_trd->plr_fifo);
                print("\nFoi removido pelo árbitro!\n", STDOUT_FILENO);
                *msg_trd->cli_msg_keep_alive = 0;
                pthread_kill(msg_trd->com_tid, SIGUSR2);
                break;
            }
            else if (msg_trd->msg->log_state == EXITED)
            {
                close(msg_trd->clt_fifo_fd);
                close(msg_trd->srv_fifo_fd);
                remove(msg_trd->plr_fifo);
                print("\nO servidor foi encerrado!\n", STDOUT_FILENO);
                *msg_trd->cli_msg_keep_alive = 0;
                pthread_kill(msg_trd->com_tid, SIGUSR2);
                break;
            }
            else if (msg_trd->msg->log_state == QUITED)
            {
                close(msg_trd->clt_fifo_fd);
                close(msg_trd->srv_fifo_fd);
                remove(msg_trd->plr_fifo);
                print("\nSaiu do jogo!\n", STDOUT_FILENO);
                *msg_trd->cli_msg_keep_alive = 0;
                pthread_kill(msg_trd->com_tid, SIGUSR2);
                break;
            }
            else
            {
                print(msg_trd->msg->msg, STDOUT_FILENO);
            }
        }
        else
        {
            print("\nDados corrompidos\n", STDIN_FILENO);
            //msg_trd->keep_alive = 0;
        }
    } while (msg_trd->keep_alive == 1);
    return NULL;
}

void sig_handler(int sig)
{
    (void)sig;
}

void *cli_thread(void *arg)
{
    char input[INPUT_SIZE] = "x"; //char array para inputs
    int log_res;            //tamanho da resposta
    CliThrd *cli_trd = (CliThrd *)arg;
    ComMsg msg;
    struct sigaction act;

    memset(&msg, 0, sizeof msg);
    msg.player_pid = cli_trd->clt_pid;

    memset(&act, 0, sizeof act);
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    act.sa_handler = sig_handler;

    sigaction(SIGUSR2, &act, NULL);

    //ciclo de leitura de comandos
    while (cli_trd->keep_alive == 1)
    {
        get_user_input(input, STDIN_FILENO, sizeof input);

        if (cli_trd->keep_alive == 1)
        {
            if (strcmp(input, "#MYGAME") == 0)
            {
                printf("O seu jogo é %s!\n>", cli_trd->game_name);
                fflush(stdout);
            }
            else
            {
                strcpy(msg.msg, input);
                msg.log_state = PLAYING;
                log_res = write(cli_trd->srv_fifo_fd, &msg, sizeof msg);

                if (log_res == -1)
                {
                    print("Aguarde pelo início do jogo...", STDOUT_FILENO);
                }
            }
        }
    }
   return NULL;
    //fim
}