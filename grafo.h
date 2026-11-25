#ifndef GRAFO_H
#define GRAFO_H

/*
 * Representação do grafo por MATRIZ DE ADJACÊNCIA.
 * adj[u][v] == 1  -> existe aresta de u para v.
 * Em grafo não direcionado a matriz é simétrica (adj[u][v] == adj[v][u]).
 */

#define MAX_VERTICES 50
#define MAX_NOME     40
#define MAX_TITULO   100

typedef struct {
    int  n;                                   /* quantidade de vértices        */
    int  num_arestas;                         /* quantidade de arestas         */
    int  direcionado;                         /* 1 = direcionado, 0 = não      */
    char titulo[MAX_TITULO];                  /* nome do grafo                 */
    char nomes[MAX_VERTICES][MAX_NOME];       /* rótulo de cada vértice        */
    int  adj[MAX_VERTICES][MAX_VERTICES];     /* matriz de adjacência          */
} Grafo;

void grafo_inicializar(Grafo *g, int direcionado);
int  grafo_adicionar_vertice(Grafo *g, const char *nome);
int  grafo_adicionar_aresta(Grafo *g, int u, int v);
int  grafo_indice(const Grafo *g, const char *nome);
int  grafo_grau(const Grafo *g, int u);
int  nomes_iguais(const char *a, const char *b);

void grafo_carregar_padrao(Grafo *g);
int  grafo_carregar_arquivo(Grafo *destino, const char *caminho);

void grafo_imprimir_resumo(const Grafo *g);
void grafo_listar_vertices(const Grafo *g);
void grafo_imprimir_lista(const Grafo *g);
void grafo_imprimir_matriz(const Grafo *g);

#endif
