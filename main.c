/*
 * ATIVIDADE AVALIATIVA 3 - Grafos em C
 * Menu, busca em profundidade (DFS) e busca em largura (BFS).
 *
 * Compilar: gcc -o grafo main.c
 * Executar: ./grafo   (no Windows: grafo.exe)
 */

#include <stdio.h>
#include <stdlib.h>

#define N 10 /* quantidade de vertices */

/* Nome de cada vertice (o numero do vertice e a posicao no vetor) */
char *nomes[N] = {
    "Centro", "Rodoviaria", "Hospital", "Escola", "Mercado",
    "Praca", "Parque", "Universidade", "Ilha", "Farol"
};

int adj[N][N];      /* matriz de adjacencia: 1 = existe aresta */

/* Dados da busca */
int visitado[N];    /* 1 = vertice ja visitado */
int caminho[N];     /* vertices na ordem em que foram visitados */
int qtdVisitados;
int qtdArestas;     /* arestas percorridas */
int encontrado;     /* 1 = objetivo encontrado */

void adicionarAresta(int u, int v)
{
    adj[u][v] = 1;
    adj[v][u] = 1; /* grafo nao direcionado */
}

void montarGrafo(void)
{
    adicionarAresta(0, 1); /* Centro - Rodoviaria       */
    adicionarAresta(0, 2); /* Centro - Hospital         */
    adicionarAresta(0, 5); /* Centro - Praca            */
    adicionarAresta(1, 4); /* Rodoviaria - Mercado      */
    adicionarAresta(2, 3); /* Hospital - Escola         */
    adicionarAresta(2, 5); /* Hospital - Praca          */
    adicionarAresta(3, 7); /* Escola - Universidade     */
    adicionarAresta(4, 7); /* Mercado - Universidade    */
    adicionarAresta(5, 6); /* Praca - Parque            */
    adicionarAresta(6, 7); /* Parque - Universidade     */
    adicionarAresta(8, 9); /* Ilha - Farol (separados do resto) */
}

void mostrarGrafo(void)
{
    int i, j;
    printf("\nGRAFO (lista de adjacencia):\n");
    for (i = 0; i < N; i++) {
        printf("%d - %-12s: ", i, nomes[i]);
        for (j = 0; j < N; j++)
            if (adj[i][j])
                printf("%s ", nomes[j]);
        printf("\n");
    }
}

void limparBusca(void)
{
    int i;
    for (i = 0; i < N; i++)
        visitado[i] = 0;
    qtdVisitados = 0;
    qtdArestas = 0;
    encontrado = 0;
}

/* DFS recursiva: desce o maximo possivel e, quando nao ha mais vizinhos
   novos, volta (backtracking) para o vertice anterior. */
void dfs(int v, int objetivo)
{
    int i;

    visitado[v] = 1;
    caminho[qtdVisitados++] = v;
    printf("Visitando %s\n", nomes[v]);

    if (v == objetivo) {
        encontrado = 1;
        return;
    }

    for (i = 0; i < N; i++) {
        if (adj[v][i] && !visitado[i]) {
            qtdArestas++;
            dfs(i, objetivo);
            if (encontrado)
                return; /* achou o objetivo: para a busca */
            printf("Voltando para %s (backtracking)\n", nomes[v]);
        }
    }
}

/* BFS com fila: visita primeiro todos os vizinhos, depois os vizinhos dos vizinhos. */
void bfs(int origem, int objetivo)
{
    int fila[N];
    int inicio = 0, fim = 0;
    int v, i;

    visitado[origem] = 1;
    fila[fim++] = origem; /* enfileira a origem */

    while (inicio < fim) {
        v = fila[inicio++]; /* desenfileira */
        caminho[qtdVisitados++] = v;
        printf("Visitando %s\n", nomes[v]);

        if (v == objetivo) {
            encontrado = 1;
            return;
        }

        for (i = 0; i < N; i++) {
            if (adj[v][i] && !visitado[i]) {
                visitado[i] = 1;
                fila[fim++] = i; /* enfileira o vizinho */
                qtdArestas++;
            }
        }
    }
}

void mostrarResultado(int origem, int objetivo)
{
    int i;

    printf("\n------ DADOS DA BUSCA ------\n");
    printf("Vertice de origem: %s\n", nomes[origem]);

    printf("Vertices visitados: ");
    for (i = 0; i < qtdVisitados; i++)
        printf("%s%s", nomes[caminho[i]], i < qtdVisitados - 1 ? ", " : "\n");

    printf("Quantidade de vertices visitados: %d\n", qtdVisitados);
    printf("Quantidade de arestas percorridas: %d\n", qtdArestas);

    printf("Caminho percorrido: ");
    for (i = 0; i < qtdVisitados; i++)
        printf("%s%s", nomes[caminho[i]], i < qtdVisitados - 1 ? " -> " : "\n");

    if (objetivo == -1)
        printf("Objetivo: nenhum (busca completa)\n");
    else if (encontrado)
        printf("Objetivo %s: ENCONTRADO\n", nomes[objetivo]);
    else
        printf("Objetivo %s: NAO ENCONTRADO\n", nomes[objetivo]);
    printf("----------------------------\n");
}

/* Le o numero de um vertice. Se podeNenhum for 1, aceita -1 (sem objetivo). */
int lerVertice(char *mensagem, int podeNenhum)
{
    int v;
    for (;;) {
        printf("%s", mensagem);
        if (scanf("%d", &v) != 1) {
            if (feof(stdin))
                exit(0);
            while (getchar() != '\n')
                ; /* descarta o que foi digitado errado */
            continue;
        }
        if ((v >= 0 && v < N) || (podeNenhum && v == -1))
            return v;
        printf("Vertice invalido.\n");
    }
}

void executarBusca(int tipo)
{
    int i, origem, objetivo;

    printf("\nVertices:\n");
    for (i = 0; i < N; i++)
        printf("  %d - %s\n", i, nomes[i]);

    origem = lerVertice("Vertice de origem: ", 0);
    objetivo = lerVertice("Vertice objetivo (-1 para nenhum): ", 1);

    limparBusca();
    if (tipo == 1) {
        printf("\n--- BUSCA EM PROFUNDIDADE (DFS) ---\n");
        dfs(origem, objetivo);
    } else {
        printf("\n--- BUSCA EM LARGURA (BFS) ---\n");
        bfs(origem, objetivo);
    }
    mostrarResultado(origem, objetivo);
}

int main(void)
{
    int opcao;

    montarGrafo();

    do {
        printf("\n===== MENU =====\n");
        printf("1 - Mostrar grafo\n");
        printf("2 - Busca em profundidade (DFS)\n");
        printf("3 - Busca em largura (BFS)\n");
        printf("0 - Sair\n");
        printf("Opcao: ");

        if (scanf("%d", &opcao) != 1) {
            if (feof(stdin))
                break;
            while (getchar() != '\n')
                ;
            opcao = -1;
        }

        switch (opcao) {
        case 1: mostrarGrafo(); break;
        case 2: executarBusca(1); break;
        case 3: executarBusca(2); break;
        case 0: printf("Saindo...\n"); break;
        default: printf("Opcao invalida.\n");
        }
    } while (opcao != 0);

    return 0;
}
