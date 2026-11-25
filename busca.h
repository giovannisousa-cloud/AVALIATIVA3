#ifndef BUSCA_H
#define BUSCA_H

#include "grafo.h"

#define BUSCA_DFS 1
#define BUSCA_BFS 2

/* Na DFS cada aresta da árvore é percorrida na ida e na volta. */
#define MAX_PASSOS (2 * MAX_VERTICES + 1)

/* Tudo o que uma busca produz, para ser exibido depois. */
typedef struct {
    int  tipo;                       /* BUSCA_DFS ou BUSCA_BFS                 */
    char algoritmo[40];              /* nome para exibição                     */
    int  origem;                     /* vértice de origem                      */
    int  objetivo;                   /* vértice procurado (-1 = nenhum)        */
    int  encontrado;                 /* 1 se o objetivo foi alcançado          */

    int  ordem[MAX_VERTICES];        /* vértices na ordem em que foram visitados */
    int  qtd_visitados;

    int  pai[MAX_VERTICES];          /* de quem cada vértice foi descoberto    */
    int  nivel[MAX_VERTICES];        /* profundidade (DFS) / distância (BFS)   */
    int  marcado[MAX_VERTICES];      /* controle interno de visitado/descoberto */

    int  arestas_percorridas;        /* arestas usadas para chegar a vértice novo */
    int  arestas_analisadas;         /* todas as arestas verificadas           */
    int  retrocessos;                /* voltas por backtracking (só DFS)       */

    int  percurso[MAX_PASSOS];       /* DFS: caminhada completa, com as voltas */
    int  percurso_volta[MAX_PASSOS]; /* 1 se o passo foi um retorno            */
    int  tam_percurso;
} ResultadoBusca;

void busca_profundidade(const Grafo *g, int origem, int objetivo,
                        int passo_a_passo, ResultadoBusca *r);
void busca_largura(const Grafo *g, int origem, int objetivo,
                   int passo_a_passo, ResultadoBusca *r);

void busca_imprimir_resultado(const Grafo *g, const ResultadoBusca *r);
void busca_comparar(const Grafo *g, int origem, int objetivo);

#endif
