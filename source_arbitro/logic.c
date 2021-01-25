
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
#include "../utils_interface.h"

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

void sig_exit(int s)
{
    (void)s;
}

void *game_thread(void *arg)
{
    GameThrd *g_trd = (GameThrd *)arg;
    ComMsg msg;
    memset(&msg, 0, sizeof msg);

    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = sig_exit;
    sigaction(SIGUSR1, &act, NULL);

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
        int exit_status;
        close(g_trd->pli->fd_pipe_write[0]);
        close(g_trd->pli->fd_pipe_read[1]);
        msg.log_state = PLAYING;

        int pipe_read = g_trd->pli->fd_pipe_read[0];
        int clt_fifo_fd = g_trd->pli->clt_fifo_fd;
        pid_t player_pid = g_trd->pli->player_pid;

        while ((nbytes = read(pipe_read, &msg.msg, sizeof msg.msg)) > 0)
        {
            msg.msg[nbytes] = '\0';
            nbytes = write(clt_fifo_fd, &msg, sizeof msg);
        }

        wait(&exit_status);
        if (WIFEXITED(exit_status))
        {
            for (int i = 0; i < g_trd->server->player_count; i++)
            {
                if (player_pid == g_trd->clients[i].player_pid)
                { 
                    g_trd->clients[i].points = WEXITSTATUS(exit_status);
                    break;                  
                }

            }
        }
    }

    return NULL;
}

void *time_handler(void *arg)
{
    TimerTrd *t = (TimerTrd *)arg;
    ComMsg msg;
    memset(&msg, 0, sizeof msg);

    pthread_mutex_lock(t->timer_mutex);
    while (*t->pause)
    {
        pthread_cond_wait(t->timer_cond, t->timer_mutex);
    }
    pthread_mutex_unlock(t->timer_mutex);

    sprintf(msg.msg, "\nO compeonato terá início dento de %ld segundos!\n>", *t->wait_time);
    if (*t->wait_time)
    {
        print(msg.msg, STDOUT_FILENO);
    }
    if (*t->wait_time > 0)
    {
        for (int i = 0; i < t->server->player_count; i++)
        {
            write(t->clients[i].clt_fifo_fd, &msg, sizeof msg);
        }
    }
    *t->countdown = true;
    sleep(*t->wait_time);

    *t->log_keep_alive = 0;
    pthread_kill(t->log_tid, SIGUSR1);
    return NULL;
}

void *login_thread(void *arg)
{
    LoginThr *l_thrd = (LoginThr *)arg;
    size_t log_res;
    PlayerLog player;
    LogState state;
    int clt_fifo_fd;
    int game_index;
    srand((unsigned int)time(NULL));
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = sig_exit;
    sigaction(SIGUSR1, &act, NULL);
    memset(&player, 0, sizeof player);

    while (l_thrd->keep_alive == 1)
    {
        int i;
        state = SUCCESS;
        log_res = read(l_thrd->server->srv_log_fifo_fd, &player, sizeof player);
        if (l_thrd->keep_alive == 1)
        {
            if (log_res != sizeof player)
            {
                fprintf(stderr, "Dados do cliente corrompidos\n");
            }

            if (l_thrd->server->n_players == l_thrd->server->player_count)
            {
                state = MAX_USERS;
            }

            if (l_thrd->server->player_count > 1)
            {
                for (i = 0; i < l_thrd->server->player_count; i++)
                {

                    if (strcmp(player.name, l_thrd->logged_users[i].name) == 0)
                    {
                        state = LOGGED;
                        break;
                    }
                }
            }

            if (state != LOGGED && state != MAX_USERS)
            {    
                l_thrd->logged_users[l_thrd->server->player_count].player_pid = player.player_pid;
                strcpy(l_thrd->logged_users[l_thrd->server->player_count].name, player.name);
                strcpy(l_thrd->logged_users[l_thrd->server->player_count].player_fifo, player.player_fifo);
                game_index = rand() % l_thrd->server->n_games;
                strncpy(l_thrd->logged_users[l_thrd->server->player_count].game_name, l_thrd->server->game_list[game_index], sizeof l_thrd->logged_users[l_thrd->server->player_count].game_name);
                l_thrd->server->player_count++;
            }

            clt_fifo_fd = open(player.player_fifo, O_WRONLY);

            if (state != LOGGED && state != MAX_USERS)
            {
                l_thrd->logged_users[l_thrd->server->player_count - 1].clt_fifo_fd = clt_fifo_fd;

                if (l_thrd->server->player_count > 1)
                {
                    pthread_mutex_lock(l_thrd->timer_mutex);
                    l_thrd->pause = false;
                    pthread_cond_signal(l_thrd->timer_cond);
                    pthread_mutex_unlock(l_thrd->timer_mutex);
                }
            }
            if (clt_fifo_fd != -1)
            {
    
            strncpy(l_thrd->logged_users[l_thrd->server->player_count - 1].game_name, l_thrd->server->game_list[game_index], sizeof l_thrd->logged_users[l_thrd->server->player_count - 1].game_name);
       

                log_res = write(clt_fifo_fd, &state, sizeof state);

                if (log_res != sizeof state)
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
        if (d->d_name[0] == 'g' && d->d_name[1] == '_')
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

int compare(const void *a, const void *b)
{
    PlayerInfo *p1 = (PlayerInfo *)a;
    PlayerInfo *p2 = (PlayerInfo *)b;
    return p2->points - p1->points;
}

void *admin_thread(void *arg)
{
    char input[INPUT_SIZE] = "x"; // char array para inputs
    AdminThread *admin = (AdminThread *)arg;
    ComMsg msg;
    struct sigaction act;
    memset(&act, 0, sizeof act);
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    act.sa_handler = sig_exit;
    sigaction(SIGUSR1, &act, NULL);
    memset(&msg, 0, sizeof msg);

    //Rotina de leitura execução de comandos do teclado
    do
    {
        print(">", STDOUT_FILENO);

        get_user_input(input, STDIN_FILENO, sizeof input);
        if (admin->keep_alive == 1)
        {

            if (strcmp(input, "PLAYERS") == 0)
            {
                if (admin->server->player_count > 0)
                {
                    print("Lista de jogadores:\n", STDOUT_FILENO);
                    for (int i = 0; i < admin->server->player_count; i++)
                    {
                        print("Nome: ", STDOUT_FILENO);
                        print(admin->clients[i].name, STDOUT_FILENO);
                        print(", jogo: ", STDOUT_FILENO);
                        print(admin->clients[i].game_name, STDOUT_FILENO);
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
                if (admin->server->n_games > 0)
                {
                    print("Lista de jogos:\n", STDOUT_FILENO);
                    for (int i = 0; i < admin->server->n_games; i++)
                    {
                        print(admin->server->game_list[i], STDOUT_FILENO);
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
                if (*admin->timer_trd->pause == true || admin->countdown == true)
                {
                    print("O comando só está disponível no decorrer do campeonato!\n", STDERR_FILENO);
                }
                else
                {
                    int i, w;
                    int exists = 0;

                    if (admin->server->wait_time != 0)
                        for (i = 0; i < admin->server->player_count; i++)
                        {
                            if (strcmp(&input[1], admin->clients[i].name) == 0)
                            {
                                exists = 1;
                                print("Jogador ", STDOUT_FILENO);
                                print(admin->clients[i].name, STDOUT_FILENO);
                                print(" removido\n", STDOUT_FILENO);
                                msg.log_state = REMOVED;
                                if ((w = write(admin->clients[i].clt_fifo_fd, &msg, sizeof msg)) == -1)
                                {
                                    perror("Erro de comunicação com o cliente");
                                }
                                if (w != sizeof msg)
                                {
                                    print("Dados corrompidos!\n", STDERR_FILENO);
                                }
                                close(admin->clients[i].clt_fifo_fd);
                                close(admin->clients[i].fd_pipe_read[0]);
                                close(admin->clients[i].fd_pipe_write[1]);
                                kill(admin->clients[i].game_pid, SIGUSR1);
                                pthread_join(admin->gtrd[i].tid, &admin->gtrd[i].retval);                                                  
                                admin->server->player_count--;                                           
                                while (i < admin->server->player_count)
                                {                              
                                    admin->clients[i] = admin->clients[i + 1];
                                    admin->gtrd[i] = admin->gtrd[i + 1];                  
                                    i++;                            
                                }
                                
                                if (admin->server->player_count < 2)
                                {
                                    print("\nNão há jogadores suficientes para continuar...\n", STDOUT_FILENO);
                                    admin->keep_alive = 0;
                                }
                                break;
                            }
                        }
                    if (!exists)
                    {
                        print("Não há jogadores com o nome indicado\n", STDOUT_FILENO);
                    }
                }
            }
            else if (strcmp(input, "END") == 0)
            {
                if (*admin->timer_trd->pause == true || admin->countdown == true)
                {
                    print("O comando só está disponível no decorrer do campeonato!\n", STDERR_FILENO);
                }
                else
                {
                    print("O campeonato foi terminado!\n", STDERR_FILENO);
                    admin->keep_alive = 0;
                }
            }
            else if (input[0] == 'S')
            {
                if (*admin->timer_trd->pause == true || admin->countdown == true)
                {
                    print("O comando só está disponível no decorrer do campeonato!\n", STDERR_FILENO);
                }
                else
                {
                    int i;
                    int exists = 0;
                    for (i = 0; i < admin->server->player_count; i++)
                    {
                        if (strcmp(&input[1], admin->clients[i].name) == 0)
                        {
                            exists = 1;
                            print("Jogador ", STDOUT_FILENO);
                            print(admin->clients[i].name, STDOUT_FILENO);
                            print(" suspenso\n", STDOUT_FILENO);
                            msg.log_state = SUSPENDED;
                            int w;
                            if ((w = write(admin->clients[i].clt_fifo_fd, &msg, sizeof msg)) == -1)
                            {
                                perror("Erro de comunicação com o cliente");
                            }
                            if (w != sizeof msg)
                            {
                                print("Dados corrompidos!\n", STDERR_FILENO);
                            }
                            if (w != sizeof msg)
                            {
                                print("Erro de comunicação com o cliente!\n", STDERR_FILENO);
                            }
                            break;
                        }
                    }
                    if (!exists)
                    {
                        print("Não há jogadores com o nome indicado\n", STDOUT_FILENO);
                    }
                }
            }
            else if (input[0] == 'R')
            {
                if (*admin->timer_trd->pause == true || admin->countdown == true)
                {
                    print("O comando só está disponível no decorrer do campeonato!\n", STDERR_FILENO);
                }
                else
                {
                    int i;
                    int exists = 0;
                    for (i = 0; i < admin->server->player_count; i++)
                    {
                        if (strcmp(&input[1], admin->clients[i].name) == 0)
                        {
                            exists = 1;
                            print("Jogador ", STDOUT_FILENO);
                            print(admin->clients[i].name, STDOUT_FILENO);
                            print(" readmitido\n", STDOUT_FILENO);
                            msg.log_state = REINSTATED;
                            int w;
                            if ((w = write(admin->clients[i].clt_fifo_fd, &msg, sizeof msg)) == -1)
                            {
                                perror("Erro de comunicação com o cliente");
                            }
                            if (w != sizeof msg)
                            {
                                print("Dados corrompidos!\n", STDERR_FILENO);
                            }
                            if (w != sizeof msg)
                            {
                                print("Erro de comunicação com o cliente!\n", STDERR_FILENO);
                            }
                            break;
                        }
                    }
                    if (!exists)
                    {
                        print("Não há jogadores com o nome indicado\n", STDOUT_FILENO);
                    }
                }
            }
            else if (strcmp(input, "EXIT") == 0)
            {
                if (admin->countdown == true)
                {
                    print("Comando não disponível durante a contagem decrescente para o início do campeonato!\n", STDERR_FILENO);
                }
                else
                {
                    pthread_mutex_lock(admin->exit_server_lock);
                    *admin->exit_server = true;
                    pthread_mutex_unlock(admin->exit_server_lock);

                    admin->keep_alive = 0;

                    if (admin->server->player_count == 1)
                    {
                        ComMsg msg;
                        memset(&msg, 0, sizeof msg);
                        msg.log_state = EXITED;

                        write(admin->clients[0].clt_fifo_fd, &msg, sizeof msg);
  
                    }
                }
            }
            else
            {
                print("Comando não reconhecido!\n", STDOUT_FILENO);
            }
        }

    } while (admin->keep_alive == 1);
    admin->login_trd->keep_alive = 0;
    *admin->timer_trd->wait_time = 0;
    pthread_mutex_lock(admin->timer_mutex);
    *admin->timer_trd->pause = false;
    pthread_cond_signal(admin->timer_cond);
    pthread_mutex_unlock(admin->timer_mutex);
    if (admin->gtime_trd->tid != 0)
    {
        pthread_kill(admin->gtime_trd->tid, SIGUSR1);
    }
    pthread_exit(NULL);
}

void *game_clt_thread(void *arg)
{
    CltMsgTrd *clt_msg = (CltMsgTrd *)arg;
    ComMsg msg;

    struct sigaction act;
    memset(&act, 0, sizeof act);
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    act.sa_handler = sig_exit;
    sigaction(SIGUSR1, &act, NULL);

    while (clt_msg->keep_alive == 1)
    {
        memset(&msg, 0, sizeof msg);
        read(clt_msg->server->srv_fifo_fd, &msg, sizeof msg);

        if (clt_msg->keep_alive == 1)
        {
            for (int i = 0; i < clt_msg->server->player_count; i++)
            {
                if (msg.player_pid == clt_msg->clients[i].player_pid)
                {

                    if (strcmp(msg.msg, "#QUIT") == 0)
                    {
                        msg.log_state = QUITED;
       
                        kill(clt_msg->clients[i].game_pid, SIGUSR1);
                        write(clt_msg->clients[i].clt_fifo_fd, &msg, sizeof msg);
                        close(clt_msg->clients[i].clt_fifo_fd);
                        close(clt_msg->clients[i].fd_pipe_read[0]);
                        close(clt_msg->clients[i].fd_pipe_write[1]);
                        clt_msg->server->player_count--;
                        
                        pthread_join(clt_msg->gtrd[i].tid, &clt_msg->gtrd[i].retval);
                     
                        while (i < clt_msg->server->player_count)
                        {                    
                            clt_msg->clients[i] = clt_msg->clients[i + 1];
                            clt_msg->gtrd[i] = clt_msg->gtrd[i + 1];     
                            i++;
                        }
                   
                      
                        if (clt_msg->server->player_count < 2)
                        {
                            print("\nNão há jogadores suficientes para continuar o jogo!\n", STDERR_FILENO);
                            clt_msg->admin_thread->keep_alive = 0;
                            pthread_kill(clt_msg->admin_thread->tid, SIGUSR1);
                            clt_msg->keep_alive = 0;
                        }
                        break;
                    }
                    else if(strcmp(msg.msg, "#MYGAME") == 0){
                        msg.log_state = PLAYING;;
                        sprintf(msg.msg, "O seu jogo é %s\n>", clt_msg->clients[i].game_name);
                        write(clt_msg->clients[i].clt_fifo_fd, &msg, sizeof msg);
                        break;
                    }
                    else
                    {
                        write(clt_msg->clients[i].fd_pipe_write[1], &msg.msg, strlen(msg.msg) + 1);              
                    }

                    break;
                }
            }
        }
    }
    return NULL;
}

void *game_timer(void *arg)
{
    GameTimerTrd *t_trd = (GameTimerTrd *)arg;

    struct sigaction act;
    memset(&act, 0, sizeof act);
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    act.sa_handler = sig_exit;
    sigaction(SIGUSR1, &act, NULL);

    sleep(t_trd->server->game_duration);
    t_trd->admin_thread->keep_alive = 0;
    pthread_kill(t_trd->admin_thread->tid, SIGUSR1);

    return NULL;
}