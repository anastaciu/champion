#make todos os executáveis
all: arbitro cliente game_000
	
#make arbitro
arbitro: arbitro_main.o arbitro_logic.o arbitro_output.o
	gcc arbitro_main.o arbitro_logic.o arbitro_output.o -o arbitro

arbitro_main.o: source_arbitro/main.c source_arbitro/defaults.h global.h source_arbitro/output_interface.h
	gcc -c -Wall -Wextra -pedantic source_arbitro/main.c -o arbitro_main.o

arbitro_logic.o: source_arbitro/logic.c
	gcc -c -Wall -Wextra -pedantic source_arbitro/logic.c -o arbitro_logic.o

arbitro_output.o: source_arbitro/output.c
	gcc -c -Wall -Wextra -pedantic source_arbitro/output.c -o arbitro_output.o

#make cliente
cliente: cliente_main.o
	gcc cliente_main.o -o cliente

cliente_main.o: source_cliente/main.c source_cliente/defaults.h  global.h
	gcc -c -Wall -Wextra -pedantic source_cliente/main.c -o cliente_main.o

#make game
game_000: game_output.o game_000_main.o game_logic.o
	gcc game_output.o game_000_main.o game_logic.o -o game_000

game_000_main.o: game_0/main.c game_0/logic_interface.h game_0/dictionary.h game_0/output_interface.h
	gcc -c -Wall -Wextra -pedantic game_0/main.c -o game_000_main.o

game_output.o: game_0/output.c
	gcc -c -Wall -Wextra -pedantic game_0/output.c -o game_output.o

game_logic.o: game_0/logic.c game_0/output_interface.h
	gcc -c -Wall -Wextra -pedantic game_0/logic.c -o game_logic.o

#apaga ficheiro objeto
clear:
	rm *.o

#apaga executáveis
rmexecs:
	rm game_000 arbitro cliente