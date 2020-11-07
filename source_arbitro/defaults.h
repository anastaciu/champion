
#define MAXPLAYER_DEFAULT 30        // número máximo de jogadores
#define MAXPLAYER_NAME "MAXPLAYER"  // nome da variável ambiente
#define GAMEDIR_DEFAULT "/"         // diretoria por defeito
#define DIR_NAME "GAMEDIR"          // nome da variável de ambiente 
#define DEFAULT_GAME_TIME 900       // tempo padrão de jogo em segundos
#define MIN_GAME_TIME 120            // tempo de jogo mínimo em segundos
#define DEFAULT_WAIT_TIME 60        // tempo padrão de espera em segundos
#define MIN_WAIT_TIME 10             // tempo mínio de espera
#define MIN_PLAYERS 2               // numero mínimo de jogadores

// Flags de erro ao receber e converter tempo de jogo e tempo de espera
enum CmdArgs {NO_ARGS, ERROR_GAME_TIME, ERROR_WAIT_TIME, ERROR_BOTH, OK};
typedef enum CmdArgs CmdArgs;

//Flags de erro para variáveis de ambiente
enum GameDirParsing{ENV_ERROR = -1, ENV_SUCCESS};
typedef enum GameDirParsing GameDirParsing;

//Estrutura para dados de configuração do servidor
typedef struct{
    time_t wait_time;
    time_t game_duration;
    char *game_dir;
    int n_players;
}ServerSettings;
