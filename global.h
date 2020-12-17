
#include <sys/types.h> // tipo pid_t

#define MAX_LEN_NAME 128 // tamanho máximo de nomes e nomes de pipes
#define OUTPUT_SIZE 2048 // tamanho máximo do output e mensagens
#define INPUT_SIZE 256   // tamanho de input para comandos

#define SERVER_LOG_FIFO "/tmp/login_fifo" // nome do FIFO do servidor para login

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
    PLAYING
} LogState; // estados de login

// estrutura simples para nome e  pontos de jogador
typedef struct
{
    char player_name[MAX_LEN_NAME]; // nome do jogador
    int points;                     // pontos
} PlrPoints;

// dados de relevantes para comunicação servidor/cliente
typedef struct
{
    int points;                   // pontos do jogador
    int player_pid;
    LogState log_state;           // estado de login
    char msg[OUTPUT_SIZE];        // mensagens de texto, contém comandos ou mensagens para o jogo
    char game_name[MAX_LEN_NAME]; // nome do jogo atribuído
    PlrPoints winner;             // dados do actual primeiro classificado
} ComMsg;

// dados da estrutura principal de comunicação cliente/servidor
typedef struct
{
    char name[MAX_LEN_NAME];        // nome do jogador
    pid_t player_pid;               // pid do processo cliente
    char player_fifo[MAX_LEN_NAME]; // nome do fifo do cliente
} PlayerLog;
