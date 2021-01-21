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
    LogState state;
    ComMsg msg;
    MsgThrd msg_trd;
    CliThrd cli_trd;
    int srv_log_fifo_fd; //descritor do fifo de login do servidor
    int srv_fifo_fd; //descritor do fifo do servidor
    int clt_fifo_fd; //descritor do fifo do cliente
    int log_res;     //tamanho da resposta

    //abre fifo de login do servidor para escrita
    srv_log_fifo_fd = open(SERVER_LOG_FIFO, O_WRONLY);

    if (srv_log_fifo_fd == -1)
    {
        perror(ERROR_SERVER_FIFO);
        remove(player.player_fifo);
        return EXIT_FAILURE;
    }
    //fim

    memset(&player, 0, sizeof player);

    //nome do jogador
    print(NAME_PROMPT_OUT, STDOUT_FILENO);
    get_user_input(player.name, STDIN_FILENO, MAX_LEN_NAME);
    //fim
    
    //nome e crição do FIFO
    sprintf(player.player_fifo, CLIENT_LOG_FIFO, player.name);
    if (mkfifo(player.player_fifo, 0777) == -1)
    {
        perror(PIPE_ERROR);
        return EXIT_FAILURE;
    }
    //fim

    //abre fifo do cliente para leitura e escrita, não bloqueante
    clt_fifo_fd = open(player.player_fifo, O_RDWR);

    if (clt_fifo_fd == -1)
    {
        perror(ERROR_CLIENT_FIFO);
        close(srv_log_fifo_fd);
        remove(player.player_fifo);
        return (EXIT_FAILURE);
    }

    player.player_pid = getpid();

    log_res = write(srv_log_fifo_fd, &player, sizeof player);

    log_res = read(clt_fifo_fd, &state, sizeof state);

    if (log_res == sizeof state)
    {
        switch (state)
        {
        case SUCCESS:
            print("Login efectuado com sucesso...\nAguarde o início do campeonato...", STDOUT_FILENO);  
            close(srv_log_fifo_fd);         
            break;
        case LOGGED:
            print("Já existe um cliente com o mesmo nome...\n", STDERR_FILENO);
            close(srv_log_fifo_fd);
            remove(player.player_fifo);
            exit(EXIT_FAILURE);
            break;
        case MAX_USERS:
            print("Atingido o número máximo de clientes, tente mais tarde...\n", STDERR_FILENO);
            close(srv_log_fifo_fd);
            remove(player.player_fifo);
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

    memset(&msg, 0, sizeof msg);

    cli_trd.keep_alive = 1;
    cli_trd.game_name = msg.game_name;
    cli_trd.srv_fifo_fd = &srv_fifo_fd;
    cli_trd.clt_pid = player.player_pid;
  
    //Criação da thread de jogo e comandos
    if (pthread_create(&cli_trd.tid, NULL, cli_thread, (void *)&cli_trd))
    {
        perror("\nErro na criação da thread");
        exit(EXIT_FAILURE);
    }
    //fim

    msg_trd.clt_fifo_fd = clt_fifo_fd;
    msg_trd.srv_fifo_fd = &srv_fifo_fd;
    msg_trd.plr_fifo = player.player_fifo;
    msg_trd.keep_alive = 1;
    msg_trd.msg = &msg;
    msg_trd.cli_msg_keep_alive = &cli_trd.keep_alive;
    msg_trd.com_tid = cli_trd.tid;
    msg_trd.clt_fifo_name = player.player_fifo;

    //Criação da thread de login
    if (pthread_create(&msg_trd.tid, NULL, com_thread, (void *)&msg_trd))
    {
        perror("\nErro na criação da thread");
        exit(EXIT_FAILURE);
    }
    //fim

    //sincronização das threads de login comunicação com o cliente
    pthread_join(msg_trd.tid, &msg_trd.retval);
    pthread_join(cli_trd.tid, &cli_trd.retval);
    //fim

    //fecha fifos de cliente e servidor, remove fifo do cliente
    close(clt_fifo_fd);
    close(srv_fifo_fd);
    remove(player.player_fifo);
    //fim
}