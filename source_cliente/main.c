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
#include "defaults.h"

int main()
{
    PlayerLog player;
    LogState log_response;
    int srv_fifo_fd;
    int clt_fifo_fd;
    size_t log_res;
    char output[OUTPUT_SIZE];
    char input[INPUT_SIZE];               //char array para inputs

    player.player_pid = getpid();
    sprintf(player.player_fifo, CLIENT_LOG_FIFO, player.player_pid);

    if (mkfifo(player.player_fifo, 0777) == -1)
    {
        perror(PIPE_ERROR);
        return EXIT_FAILURE;
    }

    srv_fifo_fd = open(SERVER_LOG_FIFO, O_WRONLY);

    if (srv_fifo_fd == -1)
    {
        perror(ERROR_SERVER_FIFO);
        unlink(player.player_fifo);
        return EXIT_FAILURE;
    }

    clt_fifo_fd = open(player.player_fifo, O_RDWR);

    if (clt_fifo_fd == -1)
    {
        perror(ERROR_CLIENT_FIFO);
        close(srv_fifo_fd);
        unlink(player.player_fifo);
        return (EXIT_FAILURE);
    }

    print(NAME_PROMPT_OUT, STDOUT_FILENO);
    get_user_input(player.name, STDIN_FILENO, MAX_LEN_NAME);
    sprintf(output, USER_NAME_OUT USER_PID_OUT, player.name, player.player_pid);
    print(output, STDOUT_FILENO);

    write(srv_fifo_fd, &player, sizeof player);

    log_res = read(clt_fifo_fd, &log_response, sizeof log_response);

    fflush(stdout);
    if (log_res == sizeof log_response)
    {
        switch (log_response)
        {
        case SUCCESS:
            print("Login efectuado com sucesso...\n", STDOUT_FILENO);
            break;
        case LOGGED:
            print("Já existe um cliente com o mesmo nome...\n", STDERR_FILENO);
            exit(EXIT_FAILURE);
            break;
        case MAX_USERS:
            print("Atingido o número máximo de clientes, tente mais tarde...\n", STDERR_FILENO);
            exit(EXIT_FAILURE);
            break;

        default:
            break;
        }
    }
    else
    {
        print("Dados corrompidos", STDERR_FILENO);
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        print(">", STDOUT_FILENO);
        fgets(input, sizeof input, stdin);
        printf("%s", input);
    }

    close(clt_fifo_fd);
    close(srv_fifo_fd);
    unlink(player.player_fifo);
    remove(player.player_fifo);
}