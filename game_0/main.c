
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include <signal.h>
#include <stdbool.h>

#include "../utils_interface.h"
#include "logic_interface.h"
#include "dictionary.h"
#include "user_interface.h"
#include "ui_constants.h"

GameData game = {0, 0, 0, 0};

//handler para signal
void sig_handler(int sig)
{
	if (sig == SIGUSR1)
	{
		if(game.points > 255)
			game.points = 255;
		exit(game.points);
	}
}

int main()
{
	char output[OUTPUT_SIZE]; //outputs vários
	Dictionary dict = {WORDS, sizeof WORDS / sizeof WORDS[0]};
	bool init = true;
	int state = 0;
	int word_index = 0;
	int old_index = 0;

	char word[WORDSIZE];	//palavra a descobrir
	char input[INPUT_SIZE]; //input do utilizador

	//index da palavra escolhida aleatoriamente
	srand(getpid());

	signal(SIGUSR1, sig_handler);

	while (true)
	{
		old_index = word_index;
		word_index = rand() % dict.size;
		game.count = 0;

		set_attempts(&game, dict.ptr[word_index]);
		fill_word(word, game.size, "_");

		if (state == WORD_FOUND)
		{
			sprintf(output, WORD_FOUND_OUT, dict.ptr[old_index], game.points);
			print_user_interface(word, game.size, SPACER, MENU, "\0", output);
		}
		else if (state == GAME_OVER)
		{
			sprintf(output, GAME_OVER_OUT, dict.ptr[old_index], game.points);
			print_user_interface(word, game.size, SPACER, MENU, "\0", output);
		}

		if (init)
		{
			print_user_interface(word, game.size, SPACER, MENU, HEADER, "\0");
			init = false;
		}

		//ciclo principal
		while (game.attempts)
		{
			size_t length = get_user_input(input, STDIN_FILENO, sizeof input);
			state = main_cycle(input, word, dict.ptr[word_index], length, &game);

			switch (state)
			{
			case HELP:
				print_user_interface(word, game.size, SPACER, MENU, HEADER, "\0");
				break;
			case REPEAT:
				print_user_interface(word, game.size, SPACER, MENU, "\0", REPEAT_OUT);
				break;
			case LETTER_NOT_FOUND:
				sprintf(output, LETTER_NOT_FOUND_OUT, input[0], game.attempts);
				print_user_interface(word, game.size, SPACER, MENU, "\0", output);
				break;
			case WORD_FOUND:
				break;
			case LETTER_FOUND:
				sprintf(output, LETTER_FOUND_OUT, input[0], game.attempts);
				print_user_interface(word, game.size, SPACER, MENU, "\0", output);
				break;
			case WRONG_WORD:
				sprintf(output, WRONG_WORD_OUT, game.attempts);
				print_user_interface(word, game.size, SPACER, MENU, "\0", output);
				break;
			case GAME_OVER:
				break;
			default:
				break;
			}
		}
	}
}