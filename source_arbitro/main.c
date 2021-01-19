#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>

#include "defaults.h"
#include "logic_interface.h"
#include "../utils_interface.h"
#include "user_interface.h"
#include "ui_constants.h"

int main(int argc, char **argv)
{
    //Cria FIFO do servidor
    if (mkfifo(SERVER_LOG_FIFO, 0777) == -1)
    {
        print(PIPE_ERROR, STDERR_FILENO);
        return EXIT_FAILURE;
    }
    //fim

    char output[OUTPUT_SIZE]; // char array para outputs vários
    ServerSettings server;    // definições do servidor
    GameDirParsing gde;       // variavel de controlo para rotina de obtenção de variável de ambiente GAMEDIR
    LoginThr login;           // estrutura para thread de login de clientes
    AdminThread admin;
    TimerTrd timer;
    ComMsg msg;
    CltMsgTrd clt_msg;
    pthread_mutex_t mutex;

    server.player_count = 0; // reset do número de jogadores ligados ao servidor

    // rotina de obtenção de argumentos da linha de comandos
    switch (command_line_arguments(&server.wait_time, &server.game_duration, argc, argv))
    {
    case NO_ARGS:
        print(NO_ARGS_OUT, STDOUT_FILENO);
        sprintf(output, ERROR_WAIT_TIME_OUT, MIN_WAIT_TIME, DEFAULT_WAIT_TIME);
        print(output, STDOUT_FILENO);
        sprintf(output, ERROR_GAME_TIME_OUT, MIN_GAME_TIME, DEFAULT_GAME_TIME);
        print(output, STDOUT_FILENO);
        print(DEFAULT_OUT, STDOUT_FILENO);
        break;
    case ERROR_GAME_TIME:
        sprintf(output, ERROR_GAME_TIME_OUT, MIN_GAME_TIME, DEFAULT_GAME_TIME);
        print(output, STDOUT_FILENO);
        print(DEFAULT_OUT, STDOUT_FILENO);
        break;
    case ERROR_WAIT_TIME:
        sprintf(output, ERROR_WAIT_TIME_OUT, MIN_WAIT_TIME, DEFAULT_WAIT_TIME);
        print(output, STDOUT_FILENO);
        print(DEFAULT_OUT, STDOUT_FILENO);
        break;
    case ERROR_BOTH:
        print(ERROR_BOTH_OUT, STDOUT_FILENO);
        sprintf(output, ERROR_WAIT_TIME_OUT, MIN_WAIT_TIME, DEFAULT_WAIT_TIME);
        print(output, STDOUT_FILENO);
        sprintf(output, ERROR_GAME_TIME_OUT, MIN_GAME_TIME, DEFAULT_GAME_TIME);
        print(output, STDOUT_FILENO);
        print(DEFAULT_OUT, STDOUT_FILENO);
        break;
    case OK:
        print(CORRECT_ARGS, STDOUT_FILENO);
        break;
    default:
        break;
    }
    sprintf(output, WAIT_TIME_OUT, server.wait_time);
    print(output, STDOUT_FILENO);
    sprintf(output, GAME_DURATION_OUT, server.game_duration);
    print(output, STDOUT_FILENO);
    //fim

    //Rotina de obtenção da variável de ambiente GAMEDIR
    if ((gde = get_game_dir(&server.game_dir)) == ENV_ERROR)
    {
        sprintf(output, GAMEDIR_ERROR_OUT, server.game_dir);
        print(output, STDOUT_FILENO);
    }
    else
    {
        sprintf(output, PARSED_GAMEDIR_OUT, server.game_dir);
        print(output, STDOUT_FILENO);
    }
    //fim

    //Procurar jogos na diretoria definida
    server.game_list = list_games(server.game_dir, &server.n_games);

    if (server.game_list == NULL || server.n_games == 0)
    {
        print("Erro, não foi possível importar jogos, diretoria definida não existe ou não tem jogos\n", STDERR_FILENO);
        remove(SERVER_LOG_FIFO);
        exit(EXIT_FAILURE);
    }
    //fim

    //Rotina de obtenção da variável de ambiente MAXPLAYER
    server.n_players = get_maxplayer();

    if (server.n_players == ENV_ERROR)
    {
        server.n_players = MAXPLAYER_DEFAULT;
        sprintf(output, MAXPLAYER_ERROR_OUT, MAXPLAYER_DEFAULT);
        print(output, STDOUT_FILENO);
    }
    else
    {
        sprintf(output, PARSED_MAXPLAYER_OUT, server.n_players);
        print(output, STDOUT_FILENO);
    }
    //fim

    PlayerInfo clients[server.n_players]; //array de clientes definido com base no número máximo de jogadores
    GameThrd gtrd[server.n_players];      // array de threads para o jogo
    memset(clients, 0, sizeof clients);

    //abre fifo do servidor para leitura e escrita
    if ((server.srv_fifo_fd = open(SERVER_LOG_FIFO, O_RDWR)) == -1)
    {
        fprintf(stderr, "Erro ao abrir FIFO\n");
        remove(SERVER_LOG_FIFO);
        return EXIT_FAILURE;
    }
    //fim

    pthread_mutex_init(&mutex, NULL);

    //Setup de dados para a thread de login
    login.keep_alive = 1;
    login.logged_users = clients;
    login.server = &server;
    server.player_count = 0;
    login.gt = gtrd;
    login.mutex = &mutex;
    //fim

    //Criação da thread de login
    if (pthread_create(&login.tid, NULL, login_thread, (void *)&login))
    {
        perror("\nErro na criação da thread");
        remove(SERVER_LOG_FIFO);
        if (gde == ENV_ERROR)
        {
            free(server.game_dir);
        }
        exit(EXIT_FAILURE);
    }
    //fim

    timer.wait_time = &server.wait_time;
    timer.log_tid = login.tid;
    timer.log_keep_alive = &login.keep_alive;


    if (pthread_create(&timer.tid, NULL, time_handler, (void *)&timer))
    {
        perror("\nErro na criação da thread");
        remove(SERVER_LOG_FIFO);
        if (gde == ENV_ERROR)
        {
            free(server.game_dir);
        }
        exit(EXIT_FAILURE);
    }

    admin.clients = clients;
    admin.gtrd = gtrd;
    admin.server = &server;
    admin.mutex = &mutex;
    admin.keep_alive = 1;

    if (pthread_create(&admin.tid, NULL, admin_thread, (void *)&admin))
    {
        perror("\nErro na criação da thread");
        remove(SERVER_LOG_FIFO);
        if (gde == ENV_ERROR)
        {
            free(server.game_dir);
        }
        exit(EXIT_FAILURE);
    }
    //fim

    pthread_join(login.tid, &login.retval);
    pthread_join(timer.tid, &timer.retval);

    memset(&msg, 0, sizeof msg);

    if (server.player_count < 2)
    {
        admin.keep_alive = 0;
        print("\nServidor encerrado, menos de 2 jogadores inscritos!\n", STDOUT_FILENO);
        if (gde == ENV_ERROR)
        {
            free(server.game_dir);
        }

        for (int i = 0; i < server.n_games; i++)
        {
            free(server.game_list[i]);
        }
        free(server.game_list);
        msg.log_state = EXITED;
        if (server.player_count == 1)
        {
            write(clients[0].clt_fifo_fd, &msg, sizeof msg);
        }
        
        close(server.srv_fifo_fd);
        remove(SERVER_LOG_FIFO);
        pthread_kill(admin.tid, SIGUSR1);
        pthread_join(admin.tid, &admin.retval);
        exit(EXIT_SUCCESS);
    }

    memset(&msg, 0, sizeof msg);

    msg.log_state = STARTED;
    strcpy(msg.msg, "\nComeçou o jogo...\n\n");

    for (int i = 0; i < server.player_count; i++)
    {

        write(clients[i].clt_fifo_fd, &msg, sizeof msg);
        gtrd[i].pli = &clients[i];
        gtrd[i].keep_alive = 1;
        gtrd[i].mutex = &mutex;
        pthread_create(&gtrd[i].tid, NULL, game_thread, (void *)&gtrd[i]);
    }

    clt_msg.keep_alive = 1;
    clt_msg.pli = clients;
    clt_msg.server = &server;
    clt_msg.gtrd = gtrd;
    clt_msg.mutex = &mutex;

    pthread_create(&clt_msg.tid, NULL, game_clt_thread, (void*) &clt_msg);

    pthread_join(admin.tid, &admin.retval);

    for (int i = 0; i < server.player_count; i++)
    {
        if (clients[i].game_pid != 0)
        {
            kill(clients[i].game_pid, SIGUSR1);
        }
        pthread_kill(gtrd[i].tid, SIGUSR1);
        msg.log_state = EXITED;
        write(clients[i].clt_fifo_fd, &msg, sizeof msg);
        close(clients[i].clt_fifo_fd);
        pthread_join(gtrd[i].tid, &gtrd[i].retval);
    }

    pthread_mutex_destroy(&mutex);

    //elimina memória reservada para game_dir caso ela tenha sido necessária
    if (gde == ENV_ERROR)
    {
        free(server.game_dir);
    }
    //fim

    //Fecha fifos abertos e elimina FIFO do servidor
    close(server.srv_fifo_fd);
    remove(SERVER_LOG_FIFO);
    //fim

    //elimina lista de jogos
    for (int i = 0; i < server.n_games; i++)
    {
        free(server.game_list[i]);
    }
    free(server.game_list);
    //fim
    print("O servidor foi encerrado\n", STDOUT_FILENO);
}