
#define MAX_LEN_NAME 50      // tamanho máximo de nomes e nome do pipe
#define OUTPUT_SIZE 150      // tamanho máximo do output e mensagens
#define INPUT_SIZE 100       // tmanho de input para comandos 

#define SERVER_LOG_FIFO "/tmp/login_fifo"     // nome do FIFO do servidor para login
#define CLIENT_LOG_FIFO "/tmp/login_%d_fifo"  // nome do FIFO de login do cliente

enum LogState{LOGGED, MAX_USERS, SUCCESS}; // estados de login
typedef enum LogState LogState;

//dados da estrutura de login do cliente
typedef struct{
    char name[MAX_LEN_NAME]; 
    pid_t player_pid;
    char player_fifo[MAX_LEN_NAME];
}PlayerLog;

//dados a enviar do servidor ao cliente
typedef struct{
    int points;
    char msg[OUTPUT_SIZE];
}PlayerMsg;



