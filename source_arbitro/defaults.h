
#include "../global.h" // MAX_LEN_NAME

#define MAXPLAYER_DEFAULT 30       // número máximo de jogadores
#define MAXPLAYER_NAME "MAXPLAYER" // nome da variável ambiente
#define DIR_NAME "GAMEDIR"         // nome da variável de ambiente
#define DEFAULT_GAME_TIME 900      // tempo padrão de jogo em segundos
#define MIN_GAME_TIME 120          // tempo de jogo mínimo em segundos
#define DEFAULT_WAIT_TIME 60       // tempo padrão de espera em segundos
#define MIN_WAIT_TIME 10           // tempo mínio de espera
#define MIN_PLAYERS 2              // numero mínimo de jogadores

// flags de erro ao receber e converter tempo de jogo e tempo de espera
enum CmdArgs
{
    NO_ARGS,
    ERROR_GAME_TIME,
    ERROR_WAIT_TIME,
    ERROR_BOTH,
    OK
};
typedef enum CmdArgs CmdArgs;

// flags de erro para variáveis de ambiente
enum GameDirParsing
{
    ENV_ERROR = -1,
    ENV_SUCCESS
};
typedef enum GameDirParsing GameDirParsing;

// estrutura para dados de configuração do servidor
typedef struct
{
    time_t wait_time;     // tempo de espera
    time_t game_duration; // tempo de duração
    char *game_dir;       // diretoria dos jogos
    int n_players;        // número máximo de jogadores
    int player_count;     // número de jogadores ligados
    int srv_fifo_fd;      // descritor do pipe do servidor
    int n_games;          // número de jogos
    char **game_list;     // lista de jogos
} ServerSettings;

// dados do cliente mantidos no servidor
typedef struct
{
    pid_t player_pid;               // pid do jogador
    char name[MAX_LEN_NAME];        // nome do jogador
    char player_fifo[MAX_LEN_NAME]; // nome do fifo do jogador
    pid_t game_pid;                 // pid do jogo atribuído
    char game_name[MAX_LEN_NAME];   // nome do jogo
    int clt_fifo_fd;                // descritor do fifo do cliente
    int points;                     // pontos do jogador
    int fd_pipe_read[2];            // unnamed pipe para ler o stdout do jogo
    int fd_pipe_write[2];           // unnamed pipe para escrever no stdin do jogo
} PlayerInfo;

//estrutura de dados usada em threads dos jogos
typedef struct
{
    int keep_alive;  // variável de controlo de execução da thread
    pthread_t tid;   // id da thread
    void *retval;    // exit status da thread
    PlayerInfo *pli; // info do jogador a passar à thread
    pthread_mutex_t *mutex;
} GameThrd;

// estrutura da thread de verificação de dados e setup de login no servidor
typedef struct
{
    int keep_alive;           // variável de controlo de execução da thread
    pthread_t tid;            // id da thread
    void *retval;             // exit status da thread
    PlayerInfo *logged_users; // array de clientes ligados
    ServerSettings *server;
    GameThrd *gt;
    pthread_mutex_t *mutex;
} LoginThr;

typedef struct
{
    pthread_t tid; // id da thread
    void *retval;  // exit status da thread
    time_t *wait_time;
    pthread_t log_tid;
} TimerTrd;

typedef struct
{
    ServerSettings *server;
    PlayerInfo *clients;
    GameThrd *gtrd;
    void *retval;  // exit status da thread
    pthread_t tid; // id da thread
    pthread_mutex_t *mutex;
} AdminThread;


typedef struct 
{
    PlayerInfo *pli;
    GameThrd *gtrd;
    ServerSettings *server;
    int keep_alive;
    pthread_t tid;
    void *retval;
    pthread_mutex_t *mutex;
} CltMsgTrd;