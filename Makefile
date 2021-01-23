#make todos os executáveis
all: arbitro cliente g_forca2 g_forca4 g_forca1 g_forca3
	
# make do arbitro
arbitro: arbitro_main.o arbitro_logic.o utils.o
	gcc -pthread arbitro_main.o arbitro_logic.o utils.o -o arbitro

arbitro_main.o: source_arbitro/main.c source_arbitro/defaults.h source_arbitro/user_interface.h global.h source_arbitro/ui_constants.h
	gcc -c -Wall -Wextra -pedantic source_arbitro/main.c -o arbitro_main.o

arbitro_logic.o: source_arbitro/logic.c source_arbitro/defaults.h global.h
	gcc -c -Wall -Wextra -pedantic source_arbitro/logic.c -o arbitro_logic.o

#make do cliente
cliente: cliente_main.o utils.o cliente_logic.o
	gcc -pthread cliente_main.o cliente_logic.o utils.o -o cliente

cliente_main.o: source_cliente/main.c source_cliente/user_interface.h global.h source_cliente/ui_constants.h source_cliente/defaults.h
	gcc -c -Wall -Wextra -pedantic source_cliente/main.c -o cliente_main.o

cliente_logic.o: source_cliente/logic.c source_cliente/defaults.h global.h source_cliente/user_interface.h source_cliente/ui_constants.h
	gcc -c -Wall -Wextra -pedantic source_cliente/logic.c -o cliente_logic.o

#make dos jogos
g_forca1: game_user.o game_000_main.o game_logic.o utils.o
	gcc game_user.o game_000_main.o game_logic.o utils.o -o g_forca1

g_forca2: game_user.o game_000_main.o game_logic.o utils.o
	gcc game_user.o game_000_main.o game_logic.o utils.o -o g_forca2

g_forca3: game_user.o game_000_main.o game_logic.o utils.o
	gcc game_user.o game_000_main.o game_logic.o utils.o -o g_forca3

g_forca4: game_user.o game_000_main.o game_logic.o utils.o
	gcc game_user.o game_000_main.o game_logic.o utils.o -o g_forca4

game_000_main.o: game_0/main.c game_0/dictionary.h game_0/user_interface.h game_0/defaults.h game_0/ui_constants.h
	gcc -c -Wall -Wextra -pedantic game_0/main.c -o game_000_main.o

game_user.o: game_0/user.c
	gcc -c -Wall -Wextra -pedantic game_0/user.c -o game_user.o

game_logic.o: game_0/logic.c game_0/defaults.h
	gcc -c -Wall -Wextra -pedantic game_0/logic.c -o game_logic.o

#ficheiro objeto com funções globais
utils.o: utils.c
	gcc -c -Wall -Wextra -pedantic utils.c -o utils.o

#apaga ficheiros objeto e executáveis
clean:
	rm g_forca4 g_forca1 g_forca2 g_forca3 arbitro cliente *.o

#apaga executáveis
rmexecs:
	rm arbitro cliente

#apaga ficheiros objeto
rmobjs:
	rm *.o