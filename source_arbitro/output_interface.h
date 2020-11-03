#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define OUTPUT_SIZE 150
#define FD_OUT fileno(stdout)

#define NO_ARGS_OUT "Argumentos não introduzidos\n"
#define ERROR_BOTH_OUT "Argumentos errados\n"
#define ERROR_GAME_TIME_OUT "Tempo de jogo incorreto\n"
#define ERROR_WAIT_TIME_OUT "Tempo de espera incorreto\n"
#define WAIT_TIME_OUT "tempo de espera: %ld\n"
#define GAME_DURATION_OUT "duração de jogo: %ld\n"

//imprime str no ficheiro com o descritor fd_out
void print(const char* str, int fd_out);
