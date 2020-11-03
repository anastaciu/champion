
#define MAXPLAYER_DEFAULT 30        // número máximo de jogadores
#define MAXPLAYER_NAME "MAXPLAYER"  // nome da variável ambiente
#define GAMEDIR_DEFAULT "/"         // diretoria por defeito
#define DIR_NAME "GAMEDIR"          // nome da variável de ambiente 
#define DEFAULT_GAME_TIME 900       // tempo padrão de jogo em segundos
#define MIN_GAME_TIME 60            // tempo de jogo mínimo em segundos
#define DEFAULT_WAIT_TIME 600       // tempo padrão de espera em segundos
#define MIN_WAIT_TIME 1             // tempo mínio de espera


#define ENVERROR -1

enum CmdArgs {NO_ARGS, ERROR_GAME_TIME, ERROR_WAIT_TIME, ERROR_BOTH, OK};
typedef enum CmdArgs CmdArgs;
