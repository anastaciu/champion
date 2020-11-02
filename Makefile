all: cliente arbitro

cliente: main_cliente.o
	gcc main_cliente.o -o cliente

arbitro: main_arbitro.o
	gcc main_arbitro.o -o arbitro

main_cliente.o: source_cliente/main_cliente.c source_cliente/defaults.h  global.h
	gcc -c -Wall -Wextra -pedantic source_cliente/main_cliente.c

main_arbitro.o: source_arbitro/main_arbitro.c source_arbitro/defaults.h  global.h
	gcc -c -Wall -Wextra -pedantic source_arbitro/main_arbitro.c

clear:
	rm *.o