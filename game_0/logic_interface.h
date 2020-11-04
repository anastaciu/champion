#include "defaults.h"

//ciclo principal, verifica o estado do jogo e age de acordo com as regras, retorna estado
int main_cycle(char *input, char *word, const char *dict_word, size_t input_length, GameData *game);

//tamanho da palavra ecolhida e tentativas
void set_attempts( GameData* game, const char* word);