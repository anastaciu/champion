#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "defaults.h"

int main_cycle(char *input, char *word, const char *dict_word, size_t input_length, GameData *game)
{
    bool found = false;
   
    //se for uma letra procura correspondências na palavra
    if (input_length == 1)
    {
        //se for ? imprime cabeçalho com regras
        if (input[0] == '?')
        {
            return HELP;
        }
        //procura letras
        for (size_t i = 0; i < game->size; i++)
        {
            //se a letra já tiver sido encontrada sai do ciclo
            if (word[i] == input[0])
            {
                return REPEAT;
            }
            //procura letras na palavra
            if (input[0] == dict_word[i])
            {
                word[i] = dict_word[i];
                found = true;
                game->count++;
            }
        }
        //se não encontra letra desconta tentativa
        if (found == false)
        {
            (game->attempts)--;
            if (game->attempts == 0)
            {
                game->points += game->count * VICTORY_LETTERS_ATTEMPTS;
                return GAME_OVER;
            }
            return LETTER_NOT_FOUND;
        }
        else
        {
            //se todas as letras encontradas termina o jogo
            if (strcmp(word, dict_word) == 0)
            {
                game->points += game->attempts * VICTORY_LETTERS_ATTEMPTS + VICTORY_LETTERS;
                game->attempts = 0;
                return WORD_FOUND;               
            }
            //se letra encontrada
            return LETTER_FOUND;
        }
    }
    //tentativa de adivinhar, input com mais do que uma letra
    else if (strcmp(input, dict_word) == 0)
    {
        game->points += game->attempts * VICTORY_ATTEMPTS + VICTORY;
        game->attempts = 0;
        return WORD_FOUND;
    }
    //tentativa de adivinhar palavra falhada
    else
    {
        (game->attempts)--;
        if (game->attempts == 0)
        {
            game->points += game->count * VICTORY_LETTERS_ATTEMPTS;
            return GAME_OVER;
        }
        return WRONG_WORD;
    }
}

void set_attempts(GameData* game, const char* word){
    game->size = strlen(word);
	game->attempts = (game->size / 2.0) + 0.5;
}