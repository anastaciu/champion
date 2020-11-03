
#define MAX_LEN_NAME 50                       // tamanho máximo nome de named pipes
#define MAX_LEN_COM 50                        // tamanho máximo argumentos da linha de comandos
#define MAX_LEN_USER 50                       // tamanho máximo do username 
#define SERVER_LOG_FIFO "/tmp/login_fifo"     // nome do FIFO do servidor para login
#define CLIENT_LOG_FIFO "/tmp/login_%d_fifo"  // nome do FIFO de login do cliente    

enum State{LOGGED};

typedef enum State State;

//dados do cliente
typedef struct{
    char nome[MAX_LEN_USER];     
    pid_t player_pid;
    size_t points;
}PlayerData;

//resposta a tentativa de login de cliente
typedef struct{
    char pipe_name[MAX_LEN_NAME];
    State state;
    pid_t server_pid;
}LogResponse;

//dados do jogo
typedef struct{
    pid_t game_pid;
    size_t points;
} GameInfo;