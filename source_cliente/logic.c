
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <errno.h>

#include "defaults.h"
#include "../utils_interface.h"
#include "ui_constants.h"

void *com_thread(void *arg)
{
    MsgThrd *msg_trd = (MsgThrd *)arg;
    bool started = false;
    LogState current = PLAYING;

    do
    {
        int r = read(msg_trd->clt_fifo_fd, msg_trd->msg, sizeof(ComMsg));
        if (r == sizeof(ComMsg))
        {
            if (msg_trd->msg->log_state == ENDED)
            {
                print("\nO campeonato terminou!\n", STDOUT_FILENO);
                print(msg_trd->msg->msg, STDOUT_FILENO);
                *msg_trd->cli_msg_keep_alive = 0;
                pthread_kill(msg_trd->com_tid, SIGUSR2);
                break;
            }

            else if (msg_trd->msg->log_state == REMOVED)
            {
                print("\nFoi removido pelo árbitro!\n", STDOUT_FILENO);
                *msg_trd->cli_msg_keep_alive = 0;
                pthread_kill(msg_trd->com_tid, SIGUSR2);
                break;
            }
            else if (msg_trd->msg->log_state == EXITED)
            {
                remove(msg_trd->clt_fifo_name);
                print("\nO servidor foi encerrado!\n", STDOUT_FILENO);
                *msg_trd->cli_msg_keep_alive = 0;
                pthread_kill(msg_trd->com_tid, SIGUSR2);                   
                break;
            }
            else if (msg_trd->msg->log_state == QUITED)
            {
                print("\nSaiu do jogo!\n", STDOUT_FILENO);
                *msg_trd->cli_msg_keep_alive = 0;
                pthread_kill(msg_trd->com_tid, SIGUSR2);
                break;
            }
            else if (msg_trd->msg->log_state == SUSPENDED)
            {
                if (current != SUSPENDED)
                {
                    close(*msg_trd->srv_fifo_fd);
                    print("\nFoi suspenso do campeonato!\nAguarde readmissão!\n>", STDOUT_FILENO);
                    current = SUSPENDED;
                }
            }
            else if (msg_trd->msg->log_state == REINSTATED)
            {
                if (current == SUSPENDED)
                {
                    *msg_trd->srv_fifo_fd = open(SERVER_LOG_FIFO, O_WRONLY);
                    print("\nFoi readmitido, pode continuar a jogar normalmente!\n>", STDOUT_FILENO);
                    *msg_trd->srv_fifo_fd = open(SERVER_FIFO, O_WRONLY);
                    current = PLAYING;
                }
            }
            else if (msg_trd->msg->log_state == STARTED)
            {

                //abre FIFO de campeonato do servidor
                if (!started)
                {
                    *msg_trd->srv_fifo_fd = open(SERVER_FIFO, O_WRONLY);

                    if (*msg_trd->srv_fifo_fd == -1)
                    {
                        perror(ERROR_SERVER_FIFO);
                        *msg_trd->cli_msg_keep_alive = 0;
                        pthread_kill(msg_trd->com_tid, SIGUSR2);
                        pthread_exit(NULL);
                    }
                    started = true;
                }
                //fim
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

int fd_is_valid(int fd)
{
    return fcntl(fd, F_GETFD) != -1 || errno != EBADF;
}

void *cli_thread(void *arg)
{
    char input[INPUT_SIZE] = "x"; //char array para inputs

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

            strcpy(msg.msg, input);
            msg.log_state = PLAYING;
            
            if (fd_is_valid(*cli_trd->srv_fifo_fd))
            {
                write(*cli_trd->srv_fifo_fd, &msg, sizeof msg);
            }
            else
            {
                print("Neste momento não é permitido comunicar com o servidor, aguarde...\n>", STDOUT_FILENO);
            }
        }
    }
    return NULL;
    //fim
}