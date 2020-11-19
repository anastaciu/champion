#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "defaults.h"
#include "logic_interface.h"
#include "../global_interface.h"
#include "../global.h"
#include "user_interface.h"
#include "ui_constants.h"

int main(int argc, char **argv)
{

    remove(SERVER_LOG_FIFO); /*TIRAR*/

    if (mkfifo(SERVER_LOG_FIFO, 0777) == -1)
    {
        print(PIPE_ERROR, STDERR_FILENO);
        return EXIT_FAILURE;
    }

    char output[OUTPUT_SIZE];
    ServerSettings server;
    GameDirParsing gde;
    int srv_fifo_fd;
    int clt_fifo_fd;
    size_t log_res;
    PlayerLog player;
    LogState log_response;

    char client_pipe[MAX_LEN_NAME];

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

    if ((gde = get_game_dir(&server.game_dir)) == ENV_ERROR)
    {
        sprintf(output, GAMEDIR_ERROR_OUT, server.game_dir);
        free(server.game_dir);
        print(output, STDOUT_FILENO);
    }
    else
    {
        sprintf(output, PARSED_GAMEDIR_OUT, server.game_dir);
        print(output, STDOUT_FILENO);
    }

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

    PlayerInfo clients[server.n_players];
    server.player_count = 0;

    if ((srv_fifo_fd = open(SERVER_LOG_FIFO, O_RDWR)) == -1)
    {
        print("Erro ao abrir FIFO", STDERR_FILENO);
        unlink(SERVER_LOG_FIFO);
        return EXIT_FAILURE;
    }

    while (1)
    {
        log_res = read(srv_fifo_fd, &player, sizeof player);
        if (log_res < sizeof player)
        {
            print("Dados corrompidos", STDERR_FILENO);
            exit(EXIT_FAILURE);
        }
        else
        {
            log_response = SUCCESS;
            if (server.n_players == server.player_count)
            {
                log_response = MAX_USERS;
            }
            if (server.player_count > 0)
            {
                for (int i = 0; i < server.player_count; i++)
                {
                    if (strcmp(player.name, clients[i].name) == 0)
                    {
                        log_response = LOGGED;
                        break;
                    }
                }
            }

            if (log_response != LOGGED && log_response != MAX_USERS)
            {               
                clients[server.player_count].payer_pid = player.player_pid;
                strcpy(clients[server.player_count].name, player.name);
                //print(clients[server.player_count].name, STDOUT_FILENO);
                //printf(" %d %d ", clients[server.player_count].payer_pid, server.player_count);
                //fflush(stdout);
                server.player_count++;


            }

            sprintf(client_pipe, CLIENT_LOG_FIFO, player.player_pid);

            clt_fifo_fd = open(client_pipe, O_WRONLY);

            if (clt_fifo_fd != -1)
            {
                log_res = write(clt_fifo_fd, &log_response, sizeof log_response);
                if (log_res == sizeof log_response)
                {
                    printf("resposta enviada com sucesso %ld bytes\n", log_res);
                    fflush(stdout);
                }
                close(clt_fifo_fd);
            }
            else
            {
                perror("Erro ao abrir FIFO do cliente");
                exit(EXIT_FAILURE);
            }

            
        }
    }

    close(srv_fifo_fd);
    unlink(SERVER_LOG_FIFO);
    remove(SERVER_LOG_FIFO);
}