
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <pthread.h>

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
    CmdArgs error1, error2, error3, error4;
    *wait_time = DEFAULT_WAIT_TIME;
    *game_duration = DEFAULT_GAME_TIME;

    if (argc > 2)
    {
        error1 = _parse_arguments("-g", argv[1], argv[2], ERROR_GAME_TIME, game_duration, DEFAULT_GAME_TIME, MIN_GAME_TIME);
        error2 = _parse_arguments("-w", argv[1], argv[2], ERROR_WAIT_TIME, wait_time, DEFAULT_WAIT_TIME, MIN_WAIT_TIME);

        if (argc > 4)
        {
            error3 = _parse_arguments("-g", argv[3], argv[4], ERROR_GAME_TIME, game_duration, DEFAULT_GAME_TIME, MIN_GAME_TIME);
            error4 = _parse_arguments("-w", argv[3], argv[4], ERROR_WAIT_TIME, wait_time, DEFAULT_WAIT_TIME, MIN_WAIT_TIME);

            if ((error1 == OK && error4 == OK) || (error2 == OK && error3 == OK))
            {
                return OK;
            }
            else if ((error1 == ERROR_GAME_TIME && error4 == ERROR_WAIT_TIME) && (error2 == ERROR_WAIT_TIME && error3 == ERROR_GAME_TIME))
            {
                return ERROR_BOTH;
            }
            else if ((error1 == ERROR_GAME_TIME && error3 == ERROR_GAME_TIME))
            {
                return ERROR_GAME_TIME;
            }
            else if ((error2 == ERROR_WAIT_TIME && error4 == ERROR_WAIT_TIME))
            {
                return ERROR_WAIT_TIME;
            }
        }
        else if (error1 == OK)
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

void *game_thread(void *arg)
{
    GameThrd *g_trd = (GameThrd *)arg;

    int nbytes;

    if (pipe(g_trd->pli->fd_pipe_read) == -1)
    {
        //erro
    }

    if (pipe(g_trd->pli->fd_pipe_write) == -1)
    {
        //erro
    }

    if ((g_trd->pli->game_pid = fork()) == -1)
    {
        //erro
    }

    if (g_trd->pli->game_pid == 0)
    {
        close(STDOUT_FILENO);
        dup(g_trd->pli->fd_pipe_read[1]);

        close(STDIN_FILENO);
        dup(g_trd->pli->fd_pipe_write[0]);

        for (int i = 2; i <= g_trd->pli->fd_pipe_write[1]; i++)
        {
            close(i);
        }

        if (execl(g_trd->pli->game_name, g_trd->pli->game_name, NULL) == -1)
        {
            //erro
        }
    }
    else
    {
        close(g_trd->pli->fd_pipe_write[0]);
        close(g_trd->pli->fd_pipe_read[1]);


        while(g_trd->keep_alive == 1)
        {
            pthread_mutex_lock(g_trd->mut);
            nbytes = read(g_trd->pli->fd_pipe_read[0], g_trd->pll->p_msg.msg, sizeof g_trd->pll->p_msg.msg);
            pthread_mutex_unlock(g_trd->mut);   
               
            if(nbytes <= 0) {
                g_trd->keep_alive = 0;
            }

            //...

            wait(&g_trd->pli->points);
            g_trd->keep_alive = 0;
        }            
    }
    return NULL;  
    
}

void *login_thread(void *arg)
{
    LoginThr *l_thrd = (LoginThr *)arg;
    size_t log_res;
    PlayerLog player;
    GameThrd gt[l_thrd->server_settings->n_players];
    int clt_fifo_fd;
    int game_index;

    memset(&player, 0, sizeof player);

    while (l_thrd->keep_alive == 1)
    {
        int i;

        if (player.p_msg.log_state != EXITED)
            log_res = read(l_thrd->server_settings->srv_fifo_fd, &player, sizeof player);

        if (log_res < sizeof player)
        {
            fprintf(stderr, "Dados do cliente corrompidos\n");
            return NULL;
        }
        else if (player.p_msg.log_state == LOGGING)
        {
            player.p_msg.log_state = SUCCESS;
            pthread_mutex_lock(l_thrd->server_settings->mut);
            if (l_thrd->server_settings->n_players == l_thrd->server_settings->player_count)
            {
                player.p_msg.log_state = MAX_USERS;
            }
            pthread_mutex_unlock(l_thrd->server_settings->mut);

            if (l_thrd->server_settings->player_count > 0)
            {
                pthread_mutex_lock(l_thrd->server_settings->mut);
                for (i = 0; i < l_thrd->server_settings->player_count; i++)
                {

                    if (strcmp(player.name, l_thrd->logged_users[i].name) == 0)
                    {
                        player.p_msg.log_state = LOGGED;
                        break;
                    }
                }
                pthread_mutex_unlock(l_thrd->server_settings->mut);
            }

            if (player.p_msg.log_state != LOGGED && player.p_msg.log_state != MAX_USERS)
            {
                pthread_mutex_lock(l_thrd->server_settings->mut);
                l_thrd->logged_users[l_thrd->server_settings->player_count].payer_pid = player.player_pid;
                strcpy(l_thrd->logged_users[l_thrd->server_settings->player_count].name, player.name);
                strcpy(l_thrd->logged_users[l_thrd->server_settings->player_count].player_fifo, player.player_fifo);
                time_t t = time(NULL);
                game_index = rand_r((unsigned int *)&t) % l_thrd->server_settings->n_games;
                strncpy(l_thrd->logged_users[l_thrd->server_settings->player_count].game_name, l_thrd->server_settings->game_list[game_index], sizeof l_thrd->logged_users[l_thrd->server_settings->player_count].game_name);

                gt[l_thrd->server_settings->player_count].pli = &l_thrd->logged_users[l_thrd->server_settings->player_count];
                gt[l_thrd->server_settings->player_count].keep_alive = 1;
                gt[l_thrd->server_settings->player_count].pll = &player;
                gt[l_thrd->server_settings->player_count].mut = l_thrd->server_settings->mut;
                pthread_mutex_unlock(l_thrd->server_settings->mut);
                if (pthread_create(&gt[l_thrd->server_settings->player_count].tid, NULL, game_thread, gt + l_thrd->server_settings->player_count))
                {
                    //erro
                }
                pthread_mutex_lock(l_thrd->server_settings->mut);
                l_thrd->server_settings->player_count++;
                pthread_mutex_unlock(l_thrd->server_settings->mut);
            }

            clt_fifo_fd = open(player.player_fifo, O_WRONLY);

            if (player.p_msg.log_state != LOGGED && player.p_msg.log_state != MAX_USERS)
            {
                pthread_mutex_lock(l_thrd->server_settings->mut);
                l_thrd->logged_users[l_thrd->server_settings->player_count - 1].clt_fifo_fd = clt_fifo_fd;
                pthread_mutex_unlock(l_thrd->server_settings->mut);
            }

            if (clt_fifo_fd != -1)
            {
                pthread_mutex_lock(l_thrd->server_settings->mut);
                strncpy(player.p_msg.game_name, l_thrd->server_settings->game_list[game_index], sizeof l_thrd->logged_users[l_thrd->server_settings->player_count - 1].game_name);
                pthread_mutex_unlock(l_thrd->server_settings->mut);
                log_res = write(clt_fifo_fd, &player, sizeof player);
                if (log_res != sizeof player)
                {
                    fprintf(stderr, "Erro na resposta ao cliente\n");
                    fflush(stdout);
                }
            }
            else
            {
                perror("Erro ao abrir FIFO do cliente");
                fflush(stdout);
            }
        }

        else if (strcmp(player.p_msg.msg, "#QUIT") == 0 /*&& player.p_msg.log_state != LOGGING*/)
        {
            bool exists = false;
            for (i = 0; i < l_thrd->server_settings->player_count; i++)
            {
                if (strcmp(l_thrd->logged_users[i].name, player.name) == 0)
                {
                    pthread_mutex_lock(l_thrd->server_settings->mut);
                    l_thrd->server_settings->player_count--;
                    pthread_mutex_unlock(l_thrd->server_settings->mut);
                    exists = true;
                    break;
                }
            }
            if (exists)
            {
                int clt_fifo = l_thrd->logged_users[i].clt_fifo_fd;

                player.p_msg.log_state = QUITED;

                write(l_thrd->logged_users[i].clt_fifo_fd, &player, sizeof player);

                while (i < l_thrd->server_settings->player_count)
                {
                    l_thrd->logged_users[i] = l_thrd->logged_users[i + 1];
                    i++;
                }
                close(clt_fifo);
            }
        }
        else
        {
            player.p_msg.log_state = SUCCESS;

            fprintf(stdout, "\nRecebida mensagem '%s' do cliente '%s'!\n>", player.p_msg.msg, player.name);
            fflush(stdout);
            char temp[strlen(player.p_msg.msg) + 50];
            sprintf(temp, "Recebida mensagem %s, nenhuma ação executada!", player.p_msg.msg);
            strcpy(player.p_msg.msg, temp);

            for (i = 0; i < l_thrd->server_settings->player_count; i++)
            {
                if (player.player_pid == l_thrd->logged_users[i].payer_pid)
                {
                    write(l_thrd->logged_users[i].clt_fifo_fd, &player, sizeof player);
                    break;
                }
            }
        }
    }
    for (int i = 0; i < l_thrd->server_settings->player_count; i++)
    {
        pthread_join(gt[i].tid, &gt[i].retval);
    }
    return NULL;
}

char **list_games(const char *path, int *n_games)
{
    char **games = malloc(1);
    int name_size;
    int i = 0;
    struct dirent *d;
    DIR *dir = opendir(path);

    if (!dir)
    {
        return NULL;
    }
    while ((d = readdir(dir)) != NULL)
    {
        if ((d->d_name[0] == 'g' && d->d_name[1] == '_') || (d->d_name[0] == 'j' && d->d_name[1] == 'o')) //"jo" para jogo de acordo com as metas, tirar na versão final
        {
            if (!(games = realloc(games, sizeof *games * (i + 1))))
            {
                return NULL;
            }
            name_size = strlen(d->d_name) + 1;
            char game_name[name_size + 2];
            sprintf(game_name, "%s%s", "./", d->d_name);
            games[i] = malloc(name_size + 2);
            memcpy(games[i], game_name, name_size + 2);
            i++;
        }
    }
    *n_games = i;
    closedir(dir);
    return games;
}
