
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include <signal.h>

#include "../global_interface.h"
#include "logic_interface.h"
#include "dictionary.h"
#include "../global.h"
#include "user_interface.h"

GameData game = {0, 0, 0, 0};

//handler para signal
void sig_handler(/*int sig*/){
	//if(sig == SIGUSR1){ // pode ser suprimido, apenas SIGUSR1 é tratado
	exit(game.points);
    //}
}

int main()
{
	char output[OUTPUT_SIZE]; //outputs vários

	Dictionary dict = {WORDS, sizeof WORDS / sizeof WORDS[0]};

	char word[WORDSIZE];  //palavra a descobrir
	char input[WORDSIZE]; //input do utilizador

	//index da palavra escolhia aleatoriamente
	srand((unsigned)time(NULL));

	signal(SIGUSR1, sig_handler);

    print(HEADER, FD_OUT);
     
	int i = 0;
	while (i++ < 3) //jogo repete 3 vezes
	{
		int word_index = rand() % dict.size;

		game.count = 0;

		set_attempts(&game, dict.ptr[word_index]);
		fill_word(word, game.size, "_");
		sleep(1);

		//ciclo principal
		while (game.attempts)
		{
			print_user_interface(word, game.size, FD_OUT, SPACER, MENU);
			size_t length = get_user_input(input, FD_IN, sizeof input);
			int state = main_cycle(input, word, dict.ptr[word_index], length, &game);

			switch (state)
			{
			case HELP:
				print(HEADER, FD_OUT);
				break;
			case REPEAT:
				print(REPEAT_OUT, FD_OUT);
				break;
			case LETTER_NOT_FOUND:
				sprintf(output, LETTER_NOT_FOUND_OUT, input[0], game.attempts);
				print(output, FD_OUT);
				break;
			case WORD_FOUND:
				sprintf(output, WORD_FOUND_OUT, dict.ptr[word_index], game.points);
				print(output, FD_OUT);
				break;
			case LETTER_FOUND:
				sprintf(output, LETTER_FOUND_OUT, input[0], game.attempts);
				print(output, FD_OUT);
				break;
			case WRONG_WORD:
				sprintf(output, WRONG_WORD_OUT, game.attempts);
				print(output, FD_OUT);
				break;
			case GAME_OVER:
				sprintf(output, GAME_OVER_OUT, dict.ptr[word_index], game.points);
				print(output, FD_OUT);
				break;
			default:
				break;
			}
		}
	}
	exit(game.points);
}