
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>

#include "interface.h"
#include "dictionary.h"
//#include "constants.h" //incluído em interface.h

int main()
{
	char output[OUTPUTSIZE]; //outputs vários

	Dictionary dict = { WORDS, sizeof WORDS / sizeof WORDS[0]};
    GameData game = {0, 0, 0, 0};

	char word[WORDSIZE];  //palavra a descobrir
	char input[WORDSIZE]; //input do utilizador

	//index da palavra escolhia aleatoriamente
	srand((unsigned)time(NULL));	
	int word_index = rand() % dict.size;

    set_attempts(&game, dict.ptr[word_index]);
	fill_word(word, game.size, "_");
	print(HEADER, FD_OUT);
	sleep(1);

	//ciclo principal
	while (game.attempts)
	{		
		print_user_interface(word, game.size, FD_OUT, SPACER, MENU) ;
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
			printf(output, FD_OUT);
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
	exit(game.points);
}