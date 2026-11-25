#ifndef FILA_H
#define FILA_H

#include "grafo.h"

/*
 * Fila circular de inteiros (índices de vértices), usada pela BFS.
 * Cada vértice entra na fila no máximo uma vez, então MAX_VERTICES basta.
 */
typedef struct {
    int itens[MAX_VERTICES];
    int inicio;   /* posição do primeiro elemento    */
    int fim;      /* posição onde entra o próximo    */
    int tamanho;  /* quantidade de elementos na fila */
} Fila;

void fila_inicializar(Fila *f);
int  fila_vazia(const Fila *f);
int  fila_enfileirar(Fila *f, int valor);
int  fila_desenfileirar(Fila *f);
void fila_imprimir(const Fila *f, const Grafo *g);

#endif
