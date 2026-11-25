CC     = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic
FONTES = main.c grafo.c fila.c busca.c

grafo: $(FONTES) grafo.h fila.h busca.h
	$(CC) $(CFLAGS) -o grafo $(FONTES)

executar: grafo
	./grafo

limpar:
	rm -f grafo grafo.exe
