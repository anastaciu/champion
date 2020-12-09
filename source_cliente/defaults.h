
#include "../global.h" // PlayerLog

#define CLIENT_LOG_FIFO "/tmp/login_%d_fifo"  // nome do FIFO de login do cliente

//estrutura da thread de interação com o servidor
typedef struct
{
    int keep_alive;     // variável de controlo de execução da thread
    pthread_t tid;      // identificador da thread
    void *retval;       // exit status da thread
    PlayerLog *p_log;   // estrutura de comunicação entre cliente e servidor
    int clt_fifo_fd;    // descritor do pipe do cliente
    int srv_fifo_fd;    // descritor do pipe do servidor
} MsgThrd;