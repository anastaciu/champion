
#define MAX_LEN_NAME 50      // tamanho máximo nomes, 48 chars + \0

//descritores de entrada e saída
#define FD_OUT fileno(stdout)
#define FD_IN fileno(stdin)

#define SERVER_LOG_FIFO "/tmp/login_fifo"     // nome do FIFO do servidor para login
#define CLIENT_LOG_FIFO "/tmp/login_%d_fifo"  // nome do FIFO de login do cliente    

enum LogState{LOGGED}; //estrutura para estados delogin

typedef enum LogState LogState;

//dados do cliente
typedef struct{
    char name[MAX_LEN_NAME];     
    pid_t player_pid;
}PlayerData;

//resposta a tentativa de login de cliente
typedef struct{
    char pipe_name[MAX_LEN_NAME];
    LogState state;
    pid_t server_pid;
}LogResponse;

//dados do jogo
typedef struct{
    pid_t game_pid;
    size_t points;
} GameInfo;