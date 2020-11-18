#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../global_interface.h"
#include "../global.h"
#include "user_interface.h"
#include "ui_constants.h"

int main()
{
    PlayerLog player;
    LogResponse log_response;

    int srv_fifo_fd;
    int clt_fifo_fd;
    int fflags;
    size_t log_res;

    char output[OUTPUT_SIZE];

    player.player_pid = getpid();

    sprintf(player.client_pipe, CLIENT_LOG_FIFO, player.player_pid);

    if (mkfifo(player.client_pipe, 0777) == -1)
    {
        perror(PIPE_ERROR);
        return EXIT_FAILURE;
    }

    if ((srv_fifo_fd = open(SERVER_LOG_FIFO, O_WRONLY)) == -1)
    {
        perror(ERROR_SERVER_FIFO);
        return EXIT_FAILURE;
    }

    if ((clt_fifo_fd = open(player.client_pipe, O_RDONLY | O_NONBLOCK)) == -1)
    {
        perror(ERROR_CLIENT_FIFO);
        close(srv_fifo_fd);
        unlink(player.client_pipe);
        return (EXIT_FAILURE);
    }

    fflags = fcntl(clt_fifo_fd, F_GETFL);
    fflags ^= O_NONBLOCK;

    print(NAME_PROMPT_OUT, STDOUT_FILENO);
    get_user_input(player.name, STDIN_FILENO, MAX_LEN_NAME);
    sprintf(output, USER_NAME_OUT USER_PID_OUT, player.name, player.player_pid);
    print(output, STDOUT_FILENO);

    write(srv_fifo_fd, &player, sizeof player);

    log_res = read(clt_fifo_fd, &log_response, sizeof log_response);

    printf("% ld, %ld ", sizeof log_response, log_res);
    fflush(stdout);
    if (log_res == sizeof log_response)
    {
        print("Login efetuado", STDOUT_FILENO);
    }
    else
    {
        print("Erro login", STDERR_FILENO);
    }

    close(clt_fifo_fd);
    close(srv_fifo_fd);
    unlink(player.client_pipe);
}