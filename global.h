
#include <sys/types.h> // tipo pid_t

#define MAX_LEN_NAME 128 // tamanho máximo de nome de cliente
#define OUTPUT_SIZE 2048 // tamanho máximo do output e mensagens
#define INPUT_SIZE 256   // tamanho de input para comandos
#define MAX_FIFO_NAME 256 // tamanho máximo de nome de cliente



#define SERVER_LOG_FIFO "/tmp/login_fifo" // nome do FIFO do servidor para login
#define SERVER_FIFO "/tmp/server_fifo" // nome do FIFO do servidor para login


typedef enum
{
    LOGGING,
    LOGGED,
    MAX_USERS,
    SUCCESS,
    REMOVED,
    SUSPENDED,
    EXITED,
    QUITED, 
    STARTED,
    PLAYING,
    REINSTATED,
    ENDED
} LogState; // estados de login

// dados de relevantes para comunicação servidor/cliente
typedef struct
{
    pid_t player_pid;             // pid do jogador para indentificação da msg no servidor
    LogState log_state;           // estado de login
    char msg[OUTPUT_SIZE];        // mensagens de texto, contém comandos ou mensagens para o jogo
} ComMsg;

// dados da estrutura principal de comunicação cliente/servidor
typedef struct
{
    char name[MAX_LEN_NAME];        // nome do jogador
    pid_t player_pid;               // pid do processo cliente
    char player_fifo[MAX_FIFO_NAME]; // nome do fifo do cliente
} PlayerLog;
