#include <stdio.h>
#include "fila.h"

void fila_inicializar(Fila *f)
{
    f->inicio = 0;
    f->fim = 0;
    f->tamanho = 0;
}

int fila_vazia(const Fila *f)
{
    return f->tamanho == 0;
}

/* Insere no final. Retorna 0 se a fila estiver cheia. */
int fila_enfileirar(Fila *f, int valor)
{
    if (f->tamanho == MAX_VERTICES)
        return 0;
    f->itens[f->fim] = valor;
    f->fim = (f->fim + 1) % MAX_VERTICES;
    f->tamanho++;
    return 1;
}

/* Remove do início. Retorna -1 se a fila estiver vazia. */
int fila_desenfileirar(Fila *f)
{
    int valor;
    if (fila_vazia(f))
        return -1;
    valor = f->itens[f->inicio];
    f->inicio = (f->inicio + 1) % MAX_VERTICES;
    f->tamanho--;
    return valor;
}

/* Mostra o conteúdo da fila do início para o fim: [ A, B, C ] */
void fila_imprimir(const Fila *f, const Grafo *g)
{
    int i;
    printf("[ ");
    for (i = 0; i < f->tamanho; i++) {
        int pos = (f->inicio + i) % MAX_VERTICES;
        printf("%s%s", i > 0 ? ", " : "", g->nomes[f->itens[pos]]);
    }
    printf(" ]\n");
}
