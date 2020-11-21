
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "defaults.h"

CmdArgs _parse_arguments(char *opt, char *argv1, char *argv2, CmdArgs flag, long *value, int def, int min)
{
    char *endptr;
    errno = 0;
    CmdArgs a = OK;

    if (strcmp(argv1, opt) == 0)
    {
        *value = strtol(argv2, &endptr, 10);
        if (errno == ERANGE || *endptr != '\0' || *value < min)
        {
            *value = def;
            a = flag;
        }
    }
    else
    {
        a = flag;
    }
    return a;
}

CmdArgs command_line_arguments(long *wait_time, long *game_duration, int argc, char **argv)
{
    CmdArgs erro1, error2, error3, error4;
    *wait_time = DEFAULT_WAIT_TIME;
    *game_duration = DEFAULT_GAME_TIME;

    if (argc > 2)
    {
        erro1 = _parse_arguments("-g", argv[1], argv[2], ERROR_GAME_TIME, game_duration, DEFAULT_GAME_TIME, MIN_GAME_TIME);
        error2 = _parse_arguments("-w", argv[1], argv[2], ERROR_WAIT_TIME, wait_time, DEFAULT_WAIT_TIME, MIN_WAIT_TIME);

        if (argc > 4)
        {
            error3 = _parse_arguments("-g", argv[3], argv[4], ERROR_GAME_TIME, game_duration, DEFAULT_GAME_TIME, MIN_GAME_TIME);
            error4 = _parse_arguments("-w", argv[3], argv[4], ERROR_WAIT_TIME, wait_time, DEFAULT_WAIT_TIME, MIN_WAIT_TIME);

            if ((erro1 == OK && error4 == OK) || (error2 == OK && error3 == OK))
            {
                return OK;
            }
            else if ((erro1 == ERROR_GAME_TIME && error4 == ERROR_WAIT_TIME) && (error2 == ERROR_WAIT_TIME && error3 == ERROR_GAME_TIME))
            {
                return ERROR_BOTH;
            }
            else if ((erro1 == ERROR_GAME_TIME && error3 == ERROR_GAME_TIME))
            {
                return ERROR_GAME_TIME;
            }
            else if ((error2 == ERROR_WAIT_TIME && error4 == ERROR_WAIT_TIME))
            {
                return ERROR_WAIT_TIME;
            }
        }
        else if (erro1 == OK)
        {
            return ERROR_WAIT_TIME;
        }
        else if (error2 == OK)
        {
            return ERROR_GAME_TIME;
        }
        else
        {
            return ERROR_BOTH;
        }
    }
    return NO_ARGS;
}

int get_maxplayer()
{
    char *endptr;
    errno = 0;
    long max_players;

    char *max_num_players = getenv(MAXPLAYER_NAME);
    if (max_num_players == NULL)
    {
        max_players = ENV_ERROR;
    }
    else
    {
        max_players = strtol(max_num_players, &endptr, 10);
        if (max_players > MAXPLAYER_DEFAULT || max_players < 2 || errno == ERANGE || *endptr != '\0')
        {
            max_players = ENV_ERROR;
        }
    }
    return max_players;
}

GameDirParsing get_game_dir(char **game_dir)
{
    *game_dir = getenv(DIR_NAME);
    if (*game_dir == NULL)
    {
        if ((*game_dir = getcwd(NULL, 0)) != NULL)
        {
            return ENV_ERROR;
        }
    }
    return ENV_SUCCESS;
}

void *login_thread(void *arg)
{
    LoginThr *l_thrd = (LoginThr *)arg;

    LogState log_response;
    size_t log_res;
    int srv_fifo_fd;
    int clt_fifo_fd;

    PlayerLog player;
    char client_pipe[MAX_LEN_NAME];

    if ((srv_fifo_fd = open(SERVER_LOG_FIFO, O_RDWR)) == -1)
    {
        fprintf(stderr, "Erro ao abrir FIFO");
        unlink(SERVER_LOG_FIFO);
        exit(EXIT_FAILURE);
    }

    while (l_thrd->keep_alive == 1)
    {
        log_res = read(srv_fifo_fd, &player, sizeof player);
        if (log_res < sizeof player)
        {
            fprintf(stderr, "Dados corrompidos");
            exit(EXIT_FAILURE);
        }
        else
        {
            log_response = SUCCESS;
            if (l_thrd->server_settings->n_players == l_thrd->server_settings->player_count)
            {
                log_response = MAX_USERS;
            }
            if (l_thrd->server_settings->player_count > 0)
            {
                for (int i = 0; i < l_thrd->server_settings->player_count; i++)
                {
                    if (strcmp(player.name, l_thrd->logged_users[i].name) == 0)
                    {
                        log_response = LOGGED;
                        break;
                    }
                }
            }

            if (log_response != LOGGED && log_response != MAX_USERS)
            {
                l_thrd->logged_users[l_thrd->server_settings->player_count].payer_pid = player.player_pid;
                strcpy(l_thrd->logged_users[l_thrd->server_settings->player_count].name, player.name);
                l_thrd->server_settings->player_count++;
            }

            sprintf(client_pipe, CLIENT_LOG_FIFO, player.player_pid);

            clt_fifo_fd = open(client_pipe, O_WRONLY);

            if (clt_fifo_fd != -1)
            {
                log_res = write(clt_fifo_fd, &log_response, sizeof log_response);
                if (log_res != sizeof log_response)
                {
                    fprintf(stderr, "Erro ao comunicar com o cliente");
                    //tratar erros
                    exit(EXIT_FAILURE);
                }
            }
        }
        close(srv_fifo_fd);
        return NULL;
    }