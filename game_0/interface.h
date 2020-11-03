#include "constants.h"

//imprime str no ficheiro com o descritor fd_out
void print(const char* str, int fd_out);

//imprime o caracter c, size numero de vezes
void print_chars(size_t size, const char* c, int fd_out);

//imprime no ficheiro com descritor fd_out com espaços
void print_word(size_t size, const char* str, int fd_out);

//imprime o interface do jogo
void print_user_interface(const char *word, size_t size, int fd, const char *spacer, const char *menu);

//verifica se ainda existem characteres no stdin, e caso se verifique, limpa-o
void _clear_buffer(int fd);

//lê linha do stdin, acrescenta o null byte e remove \n \t ou espaços do fim
size_t get_user_input(char* input, int fd, int size);

//preenche a palavra str com o character c
void fill_word(char* str, size_t size, const char* c);

//ciclo principal, verifica o estado do jogo e age de acordo com as regras, retorna estado
int main_cycle(char *input, char *word, const char *dict_word, size_t input_length, GameData *game);

//tamanho da palavra ecolhida e tentativas
void set_attempts( GameData* game, const char* word);