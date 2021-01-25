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

    char output[OUTPUT_SIZE]; // char array para outputs vários
    ServerSettings server;    // definições do servidor
    GameDirParsing gde;       // variavel de controlo para rotina de obtenção de variável de ambiente GAMEDIR
    LoginThr login;           // estrutura para thread de login de clientes
    AdminThread admin;
    TimerTrd timer;
    ComMsg msg;
    CltMsgTrd clt_msg;
    GameTimerTrd gtimer_trd;

    //trata sinal de encerramento do processo em caso de falha ao escrever em pipe
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
    {
        perror("signal");
    }

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
        sprintf(output, MAXPLAYER_ERROR_OUT, MAXPLAYER_NAME, MAXPLAYER_DEFAULT);
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
    bool exit_server;
    int global_wait_time = server.wait_time;
    int global_game_time = server.game_duration;
    pthread_mutex_t exit_server_lock;
   
    do
    {
        exit_server = false;
        pthread_mutex_t client_data_mutex;
        pthread_mutex_t timer_mutex;
        pthread_cond_t timer_cond;

        //Cria FIFO de login do servidor
        if (mkfifo(SERVER_LOG_FIFO, 0777) == -1)
        {
            print(PIPE_ERROR, STDERR_FILENO);
            return EXIT_FAILURE;
        }
        //fim

        //abre fifo do servidor para leitura e escrita
        if ((server.srv_log_fifo_fd = open(SERVER_LOG_FIFO, O_RDWR)) == -1)
        {
            fprintf(stderr, "Erro ao abrir FIFO\n");
            remove(SERVER_LOG_FIFO);
            return EXIT_FAILURE;
        }
        //fim

        //mutex para dados de clientes
        pthread_mutex_init(&client_data_mutex, NULL);

        //mutex de accesso à condição da thread timer
        pthread_mutex_init(&timer_mutex, NULL);
        pthread_cond_init(&timer_cond, NULL);

        print("Novo campeonato iniciado, aguardando inscrição de jogadores\n", STDOUT_FILENO);

        //Setup de dados para a thread de login
        memset(clients, 0, sizeof clients);
        memset(gtrd, 0, sizeof gtrd);
        server.wait_time = global_wait_time;
        server.game_duration = global_game_time;
        server.player_count = 0;
        memset(&login, 0, sizeof login);
        login.keep_alive = 1;
        login.logged_users = clients;
        login.server = &server;
        login.gt = gtrd;
        login.client_data_mutex = &client_data_mutex;
        login.timer_mutex = &timer_mutex;
        login.timer_cond = &timer_cond;
        login.pause = true;
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

        //setup de dados para a thread de controlo do tempo de espera
        memset(&timer, 0, sizeof timer);
        timer.wait_time = &server.wait_time;
        timer.log_tid = login.tid;
        timer.log_keep_alive = &login.keep_alive;
        timer.timer_mutex = &timer_mutex;
        timer.timer_cond = &timer_cond;
        timer.pause = &login.pause;
        timer.clients = clients;
        timer.server = &server;

        //fim

        //thread de controlo do tempo de espera
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
        //fim

        //Cria FIFO de jogo do servidor para o campeonato
        if (mkfifo(SERVER_FIFO, 0777) == -1)
        {
            print(PIPE_ERROR, STDERR_FILENO);
            return EXIT_FAILURE;
        }
        //fim

        //abre fifo de campeonato do servidor para leitura e escrita
        if ((server.srv_fifo_fd = open(SERVER_FIFO, O_RDWR)) == -1)
        {
            fprintf(stderr, "Erro ao abrir FIFO\n");
            remove(SERVER_FIFO);
            return EXIT_FAILURE;
        }
        //fim

        //setup dos dados para a thread de administração do servidor
        memset(&admin, 0, sizeof admin);
        memset(&gtimer_trd, 0, sizeof gtimer_trd);
        admin.clients = clients;
        admin.gtrd = gtrd;
        admin.server = &server;
        admin.client_data_mutex = &client_data_mutex;
        admin.keep_alive = 1;
        admin.timer_trd = &timer;
        admin.login_trd = &login;
        admin.exit_server = &exit_server;
        admin.exit_server_lock = &exit_server_lock;
        admin.gtime_trd = &gtimer_trd;
        admin.timer_cond = &timer_cond;
        admin.timer_mutex = &timer_mutex;
        timer.countdown = &admin.countdown;

        //fim

        //thread de administrador
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

        //fim do tempo de espera e da thread de login
        pthread_join(login.tid, &login.retval);
        pthread_join(timer.tid, &timer.retval);
        admin.countdown = false;
        //fim

        //fechar e remover FIFO de login
        close(server.srv_log_fifo_fd);
        remove(SERVER_LOG_FIFO);
        //fim

        //terminar caso o número de cilentes seja inferior a 2
        if (server.player_count < 2 && server.player_count != 0)
        {
            memset(&msg, 0, sizeof msg);
            admin.keep_alive = 0;
            print("Servidor encerrado!\n", STDOUT_FILENO);
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

            close(server.srv_fifo_fd);
            remove(SERVER_FIFO);
            admin.keep_alive = 0;
            pthread_kill(admin.tid, SIGUSR1);
            pthread_join(admin.tid, &admin.retval);
            pthread_mutex_destroy(&client_data_mutex);
            pthread_mutex_destroy(&timer_mutex);
            pthread_cond_destroy(&timer_cond);
            exit(EXIT_SUCCESS);
        }
        //fim

        //thread de controlo de tempo de jogo
        if (server.player_count > 1)
        {
            memset(&gtimer_trd, 0, sizeof gtimer_trd);
            gtimer_trd.admin_thread = &admin;
            gtimer_trd.server = &server;
            pthread_create(&gtimer_trd.tid, NULL, game_timer, (void *)&gtimer_trd);
            printf("\nCampeonato iniciado, terá a duração de %ld minutos e %ld segundos\n>", server.game_duration / 60, server.game_duration % 60);
            fflush(stdout);
        }

        //lançamento das threads dos jogos
        memset(&msg, 0, sizeof msg);
        msg.log_state = STARTED;

        for (int i = 0; i < server.player_count; i++)
        {
            sprintf(msg.msg, "\nComeçou o campeonato...\nO seu jogo é %s\n", clients[i].game_name);
            write(clients[i].clt_fifo_fd, &msg, sizeof msg);
            gtrd[i].pli = &clients[i];
            gtrd[i].keep_alive = 1;
            gtrd[i].clients = clients;
            gtrd[i].server = &server;
            gtrd[i].mutex = &client_data_mutex;
            pthread_create(&gtrd[i].tid, NULL, game_thread, (void *)&gtrd[i]);
        }
        //fim

        //thread de controlo de tempo de jogo

        gtimer_trd.admin_thread = &admin;
        gtimer_trd.server = &server;

        //pthread_create(&gtimer_trd.tid, NULL, game_timer, (void *)&gtimer_trd);

        //setup e lançamento dos dados para thread de leitura de mensagens dos clientes
        memset(&clt_msg, 0, sizeof clt_msg);
        clt_msg.keep_alive = 1;
        clt_msg.clients = clients;
        clt_msg.server = &server;
        clt_msg.gtrd = gtrd;
        clt_msg.mutex = &client_data_mutex;
        clt_msg.admin_thread = &admin;

        pthread_create(&clt_msg.tid, NULL, game_clt_thread, (void *)&clt_msg);
        //fim

        //fim da thread de administração e da thread de controlo de duração do campeonato
        pthread_join(admin.tid, &admin.retval);
        if (gtimer_trd.tid != 0)
        {
            pthread_join(gtimer_trd.tid, &gtimer_trd.retval);
        }

        //fim das threads dos jogos
        for (int i = 0; i < server.player_count; i++)
        {
            if (clients[i].game_pid != 0)
            {
                kill(clients[i].game_pid, SIGUSR1);
            }
            if (server.wait_time > 0)
            {
                msg.log_state = EXITED;

                if (write(clients[i].clt_fifo_fd, &msg, sizeof msg) == -1)
                {
                    if (errno == EPIPE)
                    {
                        perror("sinal");
                    }
                }
            }
            pthread_join(gtrd[i].tid, &gtrd[i].retval);
        }
        //fim

        //mensagens de fim de campeonato aos clientes
        if (server.wait_time == 0)
        {
            if (exit_server)
            {
                msg.log_state = EXITED;
                for (int i = 0; i < server.player_count; i++)
                {
                    write(clients[i].clt_fifo_fd, &msg, sizeof msg);
                }
            }
            else
            {
                qsort(clients, server.player_count, sizeof clients[0], compare);
                memset(&msg, 0, sizeof msg);
                msg.log_state = ENDED;

                if (server.player_count == 1)
                {
                    sprintf(msg.msg, "Não há mais jogadores. Você é vencedor com %d pontos\n", clients[0].points);
                    write(clients[0].clt_fifo_fd, &msg, sizeof msg);
                    printf("%s terminou com %d pontos\n", clients[0].name, clients[0].points);
                }
                else
                {
                    for (int i = 0; i < server.player_count; i++)
                    {

                        if (clients[0].points == 0)
                        {
                            strcpy(msg.msg, "Nenhum jogador obteve qualquer ponto\n");
                        }
                        else if (clients[i].player_pid == clients[0].player_pid)
                        {
                            sprintf(msg.msg, "Você é vencedor com %d pontos\n", clients[0].points);
                        }
                        else
                        {
                            sprintf(msg.msg, "O vencedor é o/a %s, com %d pontos\nA sua pontuação foi %d pontos\n", clients[0].name, clients[0].points, clients[i].points);
                        }
                        write(clients[i].clt_fifo_fd, &msg, sizeof msg);

                        printf("%s terminou com %d pontos\n", clients[i].name, clients[i].points);
                        fflush(stdout);
                        close(clients[i].clt_fifo_fd);
                    }
                }
            }
        }
        //fim

        //fim da thread de leitura de mensagens dos clientes
        clt_msg.keep_alive = 0;
        pthread_kill(clt_msg.tid, SIGUSR1);
        pthread_join(clt_msg.tid, &clt_msg.retval);
        pthread_mutex_destroy(&client_data_mutex);
        pthread_mutex_destroy(&timer_mutex);
        pthread_cond_destroy(&timer_cond);
        //fim

        //Fecha fifos abertos e elimina FIFO do servidor
        close(server.srv_fifo_fd);
        remove(SERVER_FIFO);
        //fim
    } while (!exit_server); //equanto não houver exit, servidor inicia novo campeonato

    //elimina memória reservada para game_dir caso ela tenha sido necessária
    if (gde == ENV_ERROR)
    {
        free(server.game_dir);
    }
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