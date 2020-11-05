
#include <stdio.h>

#define OUTPUT_SIZE 150 //tamanho máximo de mensagens de output
#define FD_OUT fileno(stdout) //descritor stdout

//mensagens de output
#define NO_ARGS_OUT "Argumentos(s) não introduzidos\n"
#define ERROR_BOTH_OUT "Argumentos incorretos\n"
#define ERROR_GAME_TIME_OUT "Tempo de jogo incorreto\n"
#define DEFAULT_OUT "Argumento(s) em falta estabelecidos por defeito\n"
#define ERROR_WAIT_TIME_OUT "Tempo de espera incorreto\n"
#define MISSING_WAIT_TIME_OUT "Tempo de espera em falta\n"
#define WAIT_TIME_OUT "Tempo de espera: %ld segundos\n"
#define GAME_DURATION_OUT "Duração de jogo: %ld segundos\n"
#define MAXPLAYER_ERROR_OUT "MAXPLAYER não encontrado ou inválido, valor '%d' difinido por defeito\n"
#define PARSED_MAXPLAYER_OUT "O número máximo de jogadores é '%d'\n"
#define PARSED_GAMEDIR_OUT "A diretoria dos jogos é '%s'\n"
#define GAMEDIR_ERROR_OUT "GAMEDIR não encontrado, '%s' é a diretoria definida por defeito\n"

