all: cliente arbitro game_000

cliente: main_cliente.o
	gcc main_cliente.o -o cliente

arbitro: main_arbitro.o
	gcc main_arbitro.o -o arbitro

game_000: user_interface.o hangman.o logic.o
	gcc user_interface.o hangman.o logic.o -o game_000

main_cliente.o: source_cliente/main_cliente.c source_cliente/defaults.h  global.h
	gcc -c -Wall -Wextra -pedantic source_cliente/main_cliente.c

main_arbitro.o: source_arbitro/main_arbitro.c source_arbitro/defaults.h  global.h
	gcc -c -Wall -Wextra -pedantic source_arbitro/main_arbitro.c

user_interface.o: game_0/user_interface.c
	gcc -c -Wall -Wextra -pedantic game_0/user_interface.c

hangman.o: game_0/hangman.c game_0/interface.h game_0/dictionary.h game_0/constants.h
	gcc -c -Wall -Wextra -pedantic game_0/hangman.c

logic.o: game_0/logic.c game_0/constants.h
	gcc -c -Wall -Wextra -pedantic game_0/logic.c

clear:
	rm *.o
