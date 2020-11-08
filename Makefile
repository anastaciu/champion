#make todos os executáveis
all: arbitro cliente jogo
	
# make do arbitro
arbitro: arbitro_main.o arbitro_logic.o global.o
	gcc arbitro_main.o arbitro_logic.o global.o -o arbitro

arbitro_main.o: source_arbitro/main.c source_arbitro/defaults.h source_arbitro/user_interface.h global.h
	gcc -c -Wall -Wextra -pedantic source_arbitro/main.c -o arbitro_main.o

arbitro_logic.o: source_arbitro/logic.c source_arbitro/defaults.h
	gcc -c -Wall -Wextra -pedantic source_arbitro/logic.c -o arbitro_logic.o

#make do cliente
cliente: cliente_main.o
	gcc cliente_main.o global.o -o cliente

cliente_main.o: source_cliente/main.c source_cliente/user_interface.h global.h
	gcc -c -Wall -Wextra -pedantic source_cliente/main.c -o cliente_main.o

#make do jogo game_000
jogo: game_user.o game_000_main.o game_logic.o global.o
	gcc game_user.o game_000_main.o game_logic.o global.o -o jogo

game_000_main.o: game_0/main.c game_0/dictionary.h game_0/user_interface.h global.h game_0/defaults.h
	gcc -c -Wall -Wextra -pedantic game_0/main.c -o game_000_main.o

game_user.o: game_0/user.c global.h
	gcc -c -Wall -Wextra -pedantic game_0/user.c -o game_user.o

game_logic.o: game_0/logic.c game_0/defaults.h
	gcc -c -Wall -Wextra -pedantic game_0/logic.c -o game_logic.o

#ficheiro objeto com funções globais
global.o: global.c
	gcc -c -Wall -Wextra -pedantic global.c -o global.o

#apaga ficheiros objeto e executáveis
clean:
	rm jogo arbitro cliente *.o

#apaga executáveis
rmexecs:
	rm jogo arbitro cliente *.o

#apaga ficheiros objeto
rmobjs:
	rm *.o