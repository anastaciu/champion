
#include "../global.h"

#define MAXPLAYER_DEFAULT 30       // número máximo de jogadores
#define MAXPLAYER_NAME "MAXPLAYER" // nome da variável ambiente
#define DIR_NAME "GAMEDIR"         // nome da variável de ambiente
#define DEFAULT_GAME_TIME 900      // tempo padrão de jogo em segundos
#define MIN_GAME_TIME 120          // tempo de jogo mínimo em segundos
#define DEFAULT_WAIT_TIME 60       // tempo padrão de espera em segundos
#define MIN_WAIT_TIME 10           // tempo mínio de espera
#define MIN_PLAYERS 2              // numero mínimo de jogadores

// Flags de erro ao receber e converter tempo de jogo e tempo de espera
enum CmdArgs
{
    NO_ARGS,
    ERROR_GAME_TIME,
    ERROR_WAIT_TIME,
    ERROR_BOTH,
    OK
};
typedef enum CmdArgs CmdArgs;

//Flags de erro para variáveis de ambiente
enum GameDirParsing
{
    ENV_ERROR = -1,
    ENV_SUCCESS
};
typedef enum GameDirParsing GameDirParsing;

//Estrutura para dados de configuração do servidor
typedef struct
{
    time_t wait_time;
    time_t game_duration;
    char *game_dir;
    int n_players;
    int player_count;
    int srv_fifo_fd;
    int n_games;
    char** game_list;
} ServerSettings;

//dados do cliente mantidos no servidor
typedef struct{
    pid_t payer_pid;
    char name[MAX_LEN_NAME];
    char player_fifo[MAX_LEN_NAME];
    pid_t game_pid;
    char game_name[MAX_LEN_NAME];
    int clt_fifo_fd;
    int points;
} PlayerInfo;

//estrutura da thread de verificação de dados e setup de login no servidor
typedef struct
{
    int keep_alive;
    pthread_t tid;
    void *retval;
    PlayerInfo *logged_users;
    ServerSettings *server_settings;
    
} LoginThr;