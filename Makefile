#make todos os executáveis
all: cliente arbitro game_000
	
#make arbitro
arbitro: main_arbitro.o logic_arbitro.o
	gcc main_arbitro.o logic_arbitro.o -o arbitro

main_arbitro.o: source_arbitro/main.c source_arbitro/defaults.h global.h
	gcc -c -Wall -Wextra -pedantic source_arbitro/main.c -o main_arbitro.o

logic_arbitro.o: source_arbitro/logic.c
	gcc -c -Wall -Wextra -pedantic source_arbitro/logic.c -o logic_arbitro.o

#make cliente
cliente: main_cliente.o
	gcc main_cliente.o -o cliente

main_cliente.o: source_cliente/main.c source_cliente/defaults.h  global.h
	gcc -c -Wall -Wextra -pedantic source_cliente/main.c -o main_cliente.o

#make game
game_000: game_output.o main_game_000.o game_logic.o
	gcc game_output.o main_game_000.o game_logic.o -o game_000

main_game_000.o: game_0/main.c game_0/logic_interface.h game_0/dictionary.h game_0/output_interface.h
	gcc -c -Wall -Wextra -pedantic game_0/main.c -o main_game_000.o

game_output.o: game_0/output.c
	gcc -c -Wall -Wextra -pedantic game_0/output.c -o game_output.o

game_logic.o: game_0/logic.c game_0/output_interface.h
	gcc -c -Wall -Wextra -pedantic game_0/logic.c -o game_logic.o

#clear files
clear:
	rm *.o
