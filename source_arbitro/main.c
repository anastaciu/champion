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
    char input[INPUT_SIZE];   // char array para inputs
    ServerSettings server;    // definições do servidor
    GameDirParsing gde;       // variavel de controlo para rotina de obtenção de variável de ambiente GAMEDIR
    LoginThr login;           // estrutura para thread de login de clientes
    PlayerLog plr;            // estrutura de comunicação cliente/servidor

    memset(&plr, 0, sizeof plr);

    server.player_count = 0; // reset do número de jogadores ligados ao servidor

    pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    server.mut = &lock;

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

    //Setup de dados para a thread de login
    login.keep_alive = 1;
    login.logged_users = clients;
    login.server_settings = &server;
    //fim

    //abre fifo do servidor para leitura e escrita
    if ((server.srv_fifo_fd = open(SERVER_LOG_FIFO, O_RDWR)) == -1)
    {
        fprintf(stderr, "Erro ao abrir FIFO\n");
        remove(SERVER_LOG_FIFO);
        return EXIT_FAILURE;
    }
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

    //Rotina de leitura execução de comandos do teclado
    do
    {
        print(">", STDOUT_FILENO);

        fflush(stdout);
        get_user_input(input, STDIN_FILENO, sizeof input);

        if (strcmp(input, "PLAYERS") == 0)
        {

            if (server.player_count > 0)
            {
                pthread_mutex_lock(&lock);
                print("Lista de jogadores:\n", STDOUT_FILENO);

                for (int i = 0; i < server.player_count; i++)
                {
                    print(clients[i].name, STDOUT_FILENO);
                    print("\n", STDOUT_FILENO);
                }
                pthread_mutex_unlock(&lock);
            }
            else
            {
                print("Não há jogadores\n", STDOUT_FILENO);
            }
        }

        else if (strcmp(input, "GAMES") == 0)
        {

            if (server.n_games > 0)
            {
                print("Lista de jogos:\n", STDOUT_FILENO);
                for (int i = 0; i < server.n_games; i++)
                {
                    print(server.game_list[i], STDOUT_FILENO);
                    print("\n", STDOUT_FILENO);
                }
            }
            else
            {
                print("Não foram carregados jogos", STDOUT_FILENO); //não deve acontecer, sem jogos o servidor não é lançado
            }
        }
        else if (input[0] == 'K')
        {
            int i;
            int exists = 0;
            pthread_mutex_lock(&lock);
            for (i = 0; i < server.player_count; i++)
            {
                if (strcmp(&input[1], clients[i].name) == 0)
                {
                    server.player_count--;
                    exists = 1;
                    break;
                }
            }
            pthread_mutex_unlock(&lock);
            if (!exists)
            {
                print("Não há jogadores com o nome indicado\n", STDOUT_FILENO);
            }
            else
            {
                plr.p_msg.log_state = REMOVED;
                //plr.p_msg.points = clients[i].points;

                pthread_mutex_lock(&lock);
                int w = write(clients[i].clt_fifo_fd, &plr, sizeof plr);
                pthread_mutex_unlock(&lock);
                if (w != sizeof plr)
                {
                    print("Erro de comunicação com o cliente!\n", STDERR_FILENO);
                }
                else
                {
                    print("Jogador ", STDOUT_FILENO);
                    print(clients[i].name, STDOUT_FILENO);
                    print(" removido\n", STDOUT_FILENO);
                    close(clients[i].clt_fifo_fd);
                    kill(clients[i].payer_pid, SIGUSR1);
                    pthread_mutex_lock(&lock);
                    while (i < server.player_count)
                    {
                        clients[i] = clients[i + 1];
                        i++;
                    }
                    pthread_mutex_unlock(&lock);
                }
            }
        }
        else if (strcmp(input, "EXIT") != 0)
        {
            print("Comando não reconhecido!\n", STDOUT_FILENO);
        }

    } while (strcmp(input, "EXIT") != 0);

    //fim

    //elimina memória reservada para game_dir caso ela tenha sido necessária
    if (gde == ENV_ERROR)
    {
        free(server.game_dir);
    }
    //fim

    //Fecha fifos abertos e elimina FIFO do servidor
    close(server.srv_fifo_fd);

    for (int i = 0; i < server.player_count; i++)
    {
        plr.p_msg.log_state = EXITED;
        write(clients[i].clt_fifo_fd, &plr, sizeof plr);
        close(clients[i].clt_fifo_fd);
        kill(clients[i].game_pid, SIGUSR1);
        kill(clients[i].payer_pid, SIGUSR1);
    }

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

    //sincronização da thread the comunicação com o cliente

    //pthread_join(login.tid, &login.retval);
    //fim
}