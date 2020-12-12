#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>

#include "../utils_interface.h"
#include "user_interface.h"
#include "logic_interface.h"
#include "ui_constants.h"
#include "defaults.h"

int main()
{
    PlayerLog player;
    //PlayerMsg player.p_msg.msg;
    MsgThrd msg_trd;
    int srv_fifo_fd; //descritor do fifo do servidor
    int clt_fifo_fd; //descritor do fifo do cliente
    size_t log_res;  //tamanho da resposta
    //char output[OUTPUT_SIZE];  //char array para outputs
    char input[INPUT_SIZE]; //char array para inputs

    memset(&player, 0, sizeof player);
    //memset(&player.p_msg.msg, 0, sizeof player.p_msg.msg);

    //Obtém pid do processo, cria nome para o fifo com base no pid, cria fifo
    player.player_pid = getpid();
    sprintf(player.player_fifo, CLIENT_LOG_FIFO, player.player_pid);
    if (mkfifo(player.player_fifo, 0777) == -1)
    {
        perror(PIPE_ERROR);
        return EXIT_FAILURE;
    }
    //fim
    //abe fifo do servidor para escrita
    srv_fifo_fd = open(SERVER_LOG_FIFO, O_WRONLY);

    if (srv_fifo_fd == -1)
    {
        perror(ERROR_SERVER_FIFO);
        remove(player.player_fifo);
        return EXIT_FAILURE;
    }
    //fim

    //abre fifo do cliente para leitura e escrita, não bloqueante
    clt_fifo_fd = open(player.player_fifo, O_RDWR);

    if (clt_fifo_fd == -1)
    {
        perror(ERROR_CLIENT_FIFO);
        close(srv_fifo_fd);
        remove(player.player_fifo);
        return (EXIT_FAILURE);
    }

    //Rotina de login no servidor com verificação da resposta e do estado de login
    print(NAME_PROMPT_OUT, STDOUT_FILENO);
    get_user_input(player.name, STDIN_FILENO, MAX_LEN_NAME);
    player.p_msg.log_state = LOGGING;
    write(srv_fifo_fd, &player, sizeof player);

    log_res = read(clt_fifo_fd, &player, sizeof player);

    if (log_res == sizeof player)
    {
        switch (player.p_msg.log_state)
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
    //fim

    //Setup de estrutura de dados da thread de comunicação com o servidor
    msg_trd.clt_fifo_fd = clt_fifo_fd;
    msg_trd.srv_fifo_fd = srv_fifo_fd;
    msg_trd.keep_alive = 1;
    msg_trd.p_log = &player;

    //Criação da thread de login
    if (pthread_create(&msg_trd.tid, NULL, com_thread, (void *)&msg_trd))
    {
        perror("\nErro na criação da thread");
        exit(EXIT_FAILURE);
    }
    //fim

    //ciclo de leitura de comandos
    while (msg_trd.keep_alive == 1)
    {
        print(">", STDOUT_FILENO);

        get_user_input(input, STDIN_FILENO, sizeof input);

        if (strcmp(input, "#MYGAME") == 0)
        {
            printf("O seu jogo é %s!\n", player.p_msg.game_name);
            fflush(stdout);
        }
        else
        {
            strcpy(player.p_msg.msg, input);

            log_res = write(srv_fifo_fd, &player, sizeof player);
            
            if (log_res != sizeof player){
                //erro
            }
        }
    }
    //fim

    //fecha fifos de cliente e servidor, remove fifo do cliente
    close(clt_fifo_fd);
    close(srv_fifo_fd);
    remove(player.player_fifo);
    //fim

    //sincronização da thread the comunicação com o cliente
    pthread_join(msg_trd.tid, &msg_trd.retval);
    //fim
}