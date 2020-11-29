#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>

#include "defaults.h"
#include "logic_interface.h"
#include "../global_interface.h"
#include "user_interface.h"
#include "ui_constants.h"

int main(int argc, char **argv)
{

    remove(SERVER_LOG_FIFO); /*DEBUG TIRAR*/

    //Cria FIFO do servidor
    if (mkfifo(SERVER_LOG_FIFO, 0777) == -1)
    {
        print(PIPE_ERROR, STDERR_FILENO);
        return EXIT_FAILURE;
    }
    //fim

    char output[OUTPUT_SIZE]; //char array para outputs vários
    char input[INPUT_SIZE];   //char array para inputs
    ServerSettings server;    //definições do servidor
    GameDirParsing gde;       //variavel de controlo para rotina de obtenção de variável de ambiente GAMEDIR
    LoginThr login;           // estrutura para thread de login de clientes
    server.player_count = 0;  //reset do número de jogadores ligados ao servidor

    //rotina de obtenção de argumentos da linha de comandos
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

    for (int i = 0; i < server.n_games; i++)
    {
        printf("%s\n", server.game_list[i]);
    }

    if (server.game_list == NULL || server.n_games == 0)
    {
        print("Erro, não foi possível importar jogos, verifique dados\n", STDERR_FILENO);
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

    PlayerInfo clients[server.n_players]; //array de clientes

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
        exit(EXIT_FAILURE);
    }
    //fim

    //Rotina de leitura de comandos do teclado
    while (1)
    {
        print(">", STDOUT_FILENO);
        fflush(stdout);
        get_user_input(input, STDIN_FILENO, sizeof input);
        if (strcmp(input, "PLAYERS") == 0)
        {
            if (server.n_players > 0)
            {
                print("Lista de jogadores:\n", STDOUT_FILENO);
                for (int i = 0; i < server.player_count; i++)
                {
                    print(clients[i].name, STDOUT_FILENO);
                    print("\n", STDOUT_FILENO);
                }
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
    }
    //fim

    //sincronização da thread the login
    login.keep_alive = 0;
    pthread_join(login.tid, &login.retval);
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
        close(clients[i].clt_fifo_fd);
    }
    unlink(SERVER_LOG_FIFO);
    remove(SERVER_LOG_FIFO);
    //fim

    //elimina lista de jogos
    for (int i = 0; i < server.n_games; i++)
    {
        free(server.game_list[i]);
    }
    free(server.game_list);
    //fim
}