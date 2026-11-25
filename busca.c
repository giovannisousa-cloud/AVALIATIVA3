#include <stdio.h>
#include <string.h>
#include "busca.h"
#include "fila.h"

/* ------------------------------------------------------------------ */
/* Auxiliares                                                          */
/* ------------------------------------------------------------------ */

static void iniciar_resultado(ResultadoBusca *r, int tipo, const char *nome,
                              int origem, int objetivo)
{
    int i;
    memset(r, 0, sizeof(*r));
    r->tipo = tipo;
    strncpy(r->algoritmo, nome, sizeof(r->algoritmo) - 1);
    r->origem = origem;
    r->objetivo = objetivo;
    for (i = 0; i < MAX_VERTICES; i++) {
        r->pai[i] = -1;
        r->nivel[i] = -1;
    }
}

static void registrar_passo(ResultadoBusca *r, int vertice, int eh_volta)
{
    if (r->tam_percurso < MAX_PASSOS) {
        r->percurso[r->tam_percurso] = vertice;
        r->percurso_volta[r->tam_percurso] = eh_volta;
        r->tam_percurso++;
    }
}

static const char *seta(const Grafo *g)
{
    return g->direcionado ? "->" : "-";
}

static void recuo(int profundidade)
{
    printf("%*s", 2 + profundidade * 4, "");
}

/* Monta o caminho origem -> destino seguindo o vetor pai. Retorna o tamanho. */
static int montar_caminho(const ResultadoBusca *r, int destino, int caminho[])
{
    int tam = 0, i, v;
    for (v = destino; v != -1 && tam < MAX_VERTICES; v = r->pai[v])
        caminho[tam++] = v;
    for (i = 0; i < tam / 2; i++) {
        int tmp = caminho[i];
        caminho[i] = caminho[tam - 1 - i];
        caminho[tam - 1 - i] = tmp;
    }
    return tam;
}

static void imprimir_sequencia(const Grafo *g, const int seq[], int tam, const char *sep)
{
    int i;
    for (i = 0; i < tam; i++)
        printf("%s%s", i > 0 ? sep : "", g->nomes[seq[i]]);
    printf("\n");
}

/* ------------------------------------------------------------------ */
/* BUSCA EM PROFUNDIDADE (DFS) - recursiva, com backtracking           */
/*                                                                     */
/* A partir de u, a busca segue para o primeiro vizinho ainda não      */
/* visitado e continua descendo. Quando u não tem mais vizinhos novos, */
/* a chamada recursiva termina e a execução VOLTA (backtracking) para  */
/* o vértice anterior, que continua testando seus próximos vizinhos.   */
/* Retorna 1 quando o objetivo é encontrado, para interromper tudo.    */
/* ------------------------------------------------------------------ */

static int dfs_recursiva(const Grafo *g, int u, int profundidade,
                         int passo_a_passo, ResultadoBusca *r)
{
    int v;

    r->marcado[u] = 1;
    r->nivel[u] = profundidade;
    r->ordem[r->qtd_visitados++] = u;

    if (passo_a_passo) {
        recuo(profundidade);
        printf("Visita %s (profundidade %d)\n", g->nomes[u], profundidade);
    }

    if (u == r->objetivo) {
        r->encontrado = 1;
        if (passo_a_passo) {
            recuo(profundidade);
            printf("*** Objetivo %s encontrado! Encerrando a busca. ***\n", g->nomes[u]);
        }
        return 1;
    }

    for (v = 0; v < g->n; v++) {
        if (!g->adj[u][v])
            continue;

        r->arestas_analisadas++;

        if (r->marcado[v]) {
            if (passo_a_passo) {
                recuo(profundidade);
                printf("  aresta %s %s %s: %s ja visitado, ignora\n",
                       g->nomes[u], seta(g), g->nomes[v], g->nomes[v]);
            }
            continue;
        }

        if (passo_a_passo) {
            recuo(profundidade);
            printf("  aresta %s %s %s: avanca\n", g->nomes[u], seta(g), g->nomes[v]);
        }

        r->pai[v] = u;
        r->arestas_percorridas++;
        registrar_passo(r, v, 0);

        if (dfs_recursiva(g, v, profundidade + 1, passo_a_passo, r))
            return 1; /* objetivo encontrado mais abaixo: para tudo */

        /* BACKTRACKING: v e seus descendentes foram esgotados, volta para u. */
        r->retrocessos++;
        registrar_passo(r, u, 1);
        if (passo_a_passo) {
            recuo(profundidade);
            printf("  <- backtracking: volta de %s para %s\n", g->nomes[v], g->nomes[u]);
        }
    }

    if (passo_a_passo) {
        recuo(profundidade);
        printf("  (todos os vizinhos de %s ja foram explorados)\n", g->nomes[u]);
    }
    return 0;
}

void busca_profundidade(const Grafo *g, int origem, int objetivo,
                        int passo_a_passo, ResultadoBusca *r)
{
    iniciar_resultado(r, BUSCA_DFS, "Busca em Profundidade (DFS)", origem, objetivo);
    registrar_passo(r, origem, 0);

    if (passo_a_passo)
        printf("\n---------- EXECUCAO PASSO A PASSO: DFS ----------\n");

    dfs_recursiva(g, origem, 0, passo_a_passo, r);
}

/* ------------------------------------------------------------------ */
/* BUSCA EM LARGURA (BFS) - com fila                                   */
/*                                                                     */
/* A origem entra na fila. Enquanto a fila não esvaziar, retira o      */
/* primeiro vértice, visita-o e coloca no fim da fila todos os seus    */
/* vizinhos ainda não descobertos. Assim os vértices são visitados por */
/* níveis: primeiro os de distância 1, depois os de distância 2 etc.   */
/* ------------------------------------------------------------------ */

void busca_largura(const Grafo *g, int origem, int objetivo,
                   int passo_a_passo, ResultadoBusca *r)
{
    Fila fila;
    int u, v;

    iniciar_resultado(r, BUSCA_BFS, "Busca em Largura (BFS)", origem, objetivo);
    fila_inicializar(&fila);

    r->marcado[origem] = 1; /* marcado = já descoberto (entrou na fila) */
    r->nivel[origem] = 0;
    fila_enfileirar(&fila, origem);

    if (passo_a_passo) {
        printf("\n---------- EXECUCAO PASSO A PASSO: BFS ----------\n");
        printf("  Enfileira a origem %s. Fila: ", g->nomes[origem]);
        fila_imprimir(&fila, g);
    }

    while (!fila_vazia(&fila)) {
        u = fila_desenfileirar(&fila);
        r->ordem[r->qtd_visitados++] = u;

        if (passo_a_passo)
            printf("\n  Desenfileira e visita %s (nivel %d)\n", g->nomes[u], r->nivel[u]);

        if (u == objetivo) {
            r->encontrado = 1;
            if (passo_a_passo)
                printf("  *** Objetivo %s encontrado! Encerrando a busca. ***\n", g->nomes[u]);
            break;
        }

        for (v = 0; v < g->n; v++) {
            if (!g->adj[u][v])
                continue;

            r->arestas_analisadas++;

            if (r->marcado[v]) {
                if (passo_a_passo)
                    printf("    aresta %s %s %s: %s ja descoberto, ignora\n",
                           g->nomes[u], seta(g), g->nomes[v], g->nomes[v]);
                continue;
            }

            r->marcado[v] = 1;
            r->pai[v] = u;
            r->nivel[v] = r->nivel[u] + 1;
            r->arestas_percorridas++;
            fila_enfileirar(&fila, v);

            if (passo_a_passo)
                printf("    aresta %s %s %s: descobre %s e enfileira\n",
                       g->nomes[u], seta(g), g->nomes[v], g->nomes[v]);
        }

        if (passo_a_passo) {
            printf("    Fila: ");
            fila_imprimir(&fila, g);
        }
    }
}

/* ------------------------------------------------------------------ */
/* Exibição dos dados gerais da busca                                  */
/* ------------------------------------------------------------------ */

static void imprimir_tabela_visita(const Grafo *g, const ResultadoBusca *r)
{
    int i;
    const char *rotulo_nivel = (r->tipo == BUSCA_DFS) ? "Profundidade" : "Nivel";

    printf("\n Tabela de visita:\n");
    printf("  +-------+------------------+------------------+--------------+\n");
    printf("  | Ordem | Vertice          | Descoberto por   | %-12s |\n", rotulo_nivel);
    printf("  +-------+------------------+------------------+--------------+\n");
    for (i = 0; i < r->qtd_visitados; i++) {
        int v = r->ordem[i];
        printf("  | %5d | %-16s | %-16s | %12d |\n", i + 1, g->nomes[v],
               r->pai[v] == -1 ? "(origem)" : g->nomes[r->pai[v]], r->nivel[v]);
    }
    printf("  +-------+------------------+------------------+--------------+\n");
}

static void imprimir_por_niveis(const Grafo *g, const ResultadoBusca *r)
{
    int nivel, i, maior = 0;

    for (i = 0; i < r->qtd_visitados; i++)
        if (r->nivel[r->ordem[i]] > maior)
            maior = r->nivel[r->ordem[i]];

    printf("\n Visita por niveis (distancia em arestas a partir da origem):\n");
    for (nivel = 0; nivel <= maior; nivel++) {
        int primeiro = 1;
        printf("   Nivel %d: ", nivel);
        for (i = 0; i < r->qtd_visitados; i++) {
            int v = r->ordem[i];
            if (r->nivel[v] == nivel) {
                printf("%s%s", primeiro ? "" : ", ", g->nomes[v]);
                primeiro = 0;
            }
        }
        printf("\n");
    }
}

static void imprimir_percurso_dfs(const Grafo *g, const ResultadoBusca *r)
{
    int i;
    printf("\n Percurso completo com backtracking ( -> avanca | <~ volta ):\n   ");
    for (i = 0; i < r->tam_percurso; i++) {
        if (i > 0)
            printf("%s", r->percurso_volta[i] ? " <~ " : " -> ");
        printf("%s", g->nomes[r->percurso[i]]);
    }
    printf("\n");
}

void busca_imprimir_resultado(const Grafo *g, const ResultadoBusca *r)
{
    int caminho[MAX_VERTICES];
    int visitado[MAX_VERTICES];
    int i, tam, faltando = 0;

    memset(visitado, 0, sizeof(visitado));
    for (i = 0; i < r->qtd_visitados; i++)
        visitado[r->ordem[i]] = 1;

    printf("\n================ DADOS GERAIS DA BUSCA ================\n");
    printf(" Algoritmo .....................: %s\n", r->algoritmo);
    printf(" Vertice de origem .............: %s\n", g->nomes[r->origem]);

    if (r->objetivo >= 0)
        printf(" Vertice objetivo ..............: %s -> %s\n", g->nomes[r->objetivo],
               r->encontrado ? "ENCONTRADO" : "NAO ENCONTRADO");
    else
        printf(" Vertice objetivo ..............: nenhum (busca completa)\n");

    printf(" Vertices visitados ............: ");
    imprimir_sequencia(g, r->ordem, r->qtd_visitados, ", ");
    printf(" Quantidade de vertices visitados: %d de %d\n", r->qtd_visitados, g->n);
    printf(" Arestas percorridas ...........: %d (usadas para chegar a vertices novos)\n",
           r->arestas_percorridas);
    printf(" Arestas analisadas ............: %d (todas as verificacoes de vizinhos)\n",
           r->arestas_analisadas);
    if (r->tipo == BUSCA_DFS)
        printf(" Retrocessos (backtracking) ....: %d\n", r->retrocessos);

    printf("\n Caminho percorrido (ordem de visita):\n   ");
    imprimir_sequencia(g, r->ordem, r->qtd_visitados, " -> ");

    if (r->tipo == BUSCA_DFS)
        imprimir_percurso_dfs(g, r);
    else
        imprimir_por_niveis(g, r);

    imprimir_tabela_visita(g, r);

    if (r->objetivo >= 0) {
        if (r->encontrado) {
            tam = montar_caminho(r, r->objetivo, caminho);
            printf("\n Caminho da origem ate o objetivo (%d aresta%s)%s:\n   ", tam - 1,
                   tam - 1 == 1 ? "" : "s",
                   r->tipo == BUSCA_BFS ? " - menor caminho em numero de arestas" : "");
            imprimir_sequencia(g, caminho, tam, " -> ");
        } else {
            printf("\n O objetivo %s NAO foi encontrado: nao existe caminho de %s ate ele.\n",
                   g->nomes[r->objetivo], g->nomes[r->origem]);
        }
    }

    if (r->objetivo < 0 || !r->encontrado) {
        printf("\n Vertices nao alcancados a partir da origem: ");
        for (i = 0; i < g->n; i++) {
            if (!visitado[i]) {
                printf("%s%s", faltando ? ", " : "", g->nomes[i]);
                faltando++;
            }
        }
        printf("%s\n", faltando ? "" : "nenhum (todos foram visitados)");
    }
    printf("=======================================================\n");
}

/* ------------------------------------------------------------------ */
/* Comparação lado a lado entre DFS e BFS                              */
/* ------------------------------------------------------------------ */

void busca_comparar(const Grafo *g, int origem, int objetivo)
{
    static ResultadoBusca dfs, bfs;
    int caminho[MAX_VERTICES];
    char texto_dfs[32], texto_bfs[32];

    busca_profundidade(g, origem, objetivo, 0, &dfs);
    busca_largura(g, origem, objetivo, 0, &bfs);

    printf("\n================ COMPARACAO DFS x BFS ================\n");
    printf(" Origem: %s | Objetivo: %s\n\n", g->nomes[origem],
           objetivo >= 0 ? g->nomes[objetivo] : "nenhum (busca completa)");

    printf("  %-28s %-12s %-12s\n", "", "DFS", "BFS");
    printf("  %-28s %-12d %-12d\n", "Vertices visitados", dfs.qtd_visitados, bfs.qtd_visitados);
    printf("  %-28s %-12d %-12d\n", "Arestas percorridas", dfs.arestas_percorridas,
           bfs.arestas_percorridas);
    printf("  %-28s %-12d %-12d\n", "Arestas analisadas", dfs.arestas_analisadas,
           bfs.arestas_analisadas);
    printf("  %-28s %-12d %-12s\n", "Retrocessos (backtracking)", dfs.retrocessos, "-");

    if (objetivo >= 0) {
        printf("  %-28s %-12s %-12s\n", "Objetivo",
               dfs.encontrado ? "ENCONTRADO" : "NAO ENCONTR.",
               bfs.encontrado ? "ENCONTRADO" : "NAO ENCONTR.");
        if (dfs.encontrado)
            sprintf(texto_dfs, "%d arestas", montar_caminho(&dfs, objetivo, caminho) - 1);
        else
            strcpy(texto_dfs, "-");
        if (bfs.encontrado)
            sprintf(texto_bfs, "%d arestas", montar_caminho(&bfs, objetivo, caminho) - 1);
        else
            strcpy(texto_bfs, "-");
        printf("  %-28s %-12s %-12s\n", "Tamanho do caminho", texto_dfs, texto_bfs);
    }

    printf("\n  Ordem DFS: ");
    imprimir_sequencia(g, dfs.ordem, dfs.qtd_visitados, " -> ");
    printf("  Ordem BFS: ");
    imprimir_sequencia(g, bfs.ordem, bfs.qtd_visitados, " -> ");

    if (objetivo >= 0 && dfs.encontrado && bfs.encontrado) {
        printf("\n  Caminho DFS: ");
        imprimir_sequencia(g, caminho, montar_caminho(&dfs, objetivo, caminho), " -> ");
        printf("  Caminho BFS: ");
        imprimir_sequencia(g, caminho, montar_caminho(&bfs, objetivo, caminho), " -> ");
    }
    printf("======================================================\n");
}
