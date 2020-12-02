
#include "../global.h"

#define CLIENT_LOG_FIFO "/tmp/login_%d_fifo"  // nome do FIFO de login do cliente


//estrutura da thread de comunicação com o servidor
typedef struct
{
    int keep_alive;
    pthread_t tid;
    void *retval;
    PlayerMsg *msg;
    int clt_fifo_fd;
    int srv_fifo_fd;
    char clt_fifo_name[MAX_LEN_NAME];    
    
} MsgThrd;