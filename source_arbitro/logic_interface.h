#include <stdbool.h>

//função helper da função command_line_arguments
CmdArgs _parse_arguments(char *opt, char *argv1, char *argv2, CmdArgs flag, long *value, int def, int min);

//Verifica argumentos da linha de comandos e estabelece valores para tempo de espera e tempo de jogo retorna flags de erro específicas
CmdArgs command_line_arguments(long *wait_time, long *duration, int argc, char **argv);

//Verifica variável de ambiente MAXPLAYER e estabelece o seu valor, retorna -1 se valor for inválido
int get_maxplayer();

////Verifica variável de ambiente MAXPLAYER e estabelece o seu valor retorna flags de sucesso ou erro
GameDirParsing get_game_dir(char **game_dir);

//Cria named pipe do servidor para login de clientes
bool make_login_fifo(const char *pipe_name);