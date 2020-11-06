
#define MAX_LEN_NAME 50      // tamanho máximo de nomes
#define OUTPUT_SIZE 150       // tamanho máximo do output e mensagens

//descritores de entrada e saída para stdin e stdout
#define FD_OUT fileno(stdout)
#define FD_IN fileno(stdin)

#define SERVER_LOG_FIFO "/tmp/login_fifo"     // nome do FIFO do servidor para login
#define CLIENT_LOG_FIFO "/tmp/login_%d_fifo"  // nome do FIFO de login do cliente    

enum LogState{LOGGED, MAX_USERS}; // estados de login
typedef enum LogState LogState;

//dados da estrutura de login do cliente
typedef struct{
    char name[MAX_LEN_NAME];     
    pid_t player_pid;
}PlayerLog;

//resposta a tentativa de login de cliente
typedef struct{
    char pipe_name[MAX_LEN_NAME];
    LogState state;
}LogResponse;

//dados a enviar do servidor ao cliente
typedef struct{
    int points;
    char msg[OUTPUT_SIZE];
}PlayerMsg;

//dados do cliente mantidos no servidor
typedef struct{
    pid_t payer_pid;
    pid_t game_pid;
    char client_pipe[MAX_LEN_NAME];
    PlayerMsg p_msg;
} GameInfo;
