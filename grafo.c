#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "grafo.h"

/* ------------------------------------------------------------------ */
/* Operações básicas                                                   */
/* ------------------------------------------------------------------ */

void grafo_inicializar(Grafo *g, int direcionado)
{
    memset(g, 0, sizeof(*g));
    g->direcionado = direcionado;
    strcpy(g->titulo, "Grafo sem titulo");
}

/* Compara dois nomes ignorando maiúsculas/minúsculas. */
int nomes_iguais(const char *a, const char *b)
{
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b))
            return 0;
        a++;
        b++;
    }
    return *a == *b;
}

/* Retorna o índice do vértice com esse nome, ou -1 se não existir. */
int grafo_indice(const Grafo *g, const char *nome)
{
    int i;
    for (i = 0; i < g->n; i++)
        if (nomes_iguais(g->nomes[i], nome))
            return i;
    return -1;
}

/* Retorna o índice do novo vértice, ou -1 (cheio ou nome repetido). */
int grafo_adicionar_vertice(Grafo *g, const char *nome)
{
    if (g->n >= MAX_VERTICES || grafo_indice(g, nome) != -1)
        return -1;
    strncpy(g->nomes[g->n], nome, MAX_NOME - 1);
    g->nomes[g->n][MAX_NOME - 1] = '\0';
    return g->n++;
}

/* Retorna 1 se a aresta foi criada, 0 se inválida ou repetida. */
int grafo_adicionar_aresta(Grafo *g, int u, int v)
{
    if (u < 0 || v < 0 || u >= g->n || v >= g->n)
        return 0;
    if (g->adj[u][v])
        return 0;
    g->adj[u][v] = 1;
    if (!g->direcionado)
        g->adj[v][u] = 1;
    g->num_arestas++;
    return 1;
}

int grafo_grau(const Grafo *g, int u)
{
    int v, grau = 0;
    for (v = 0; v < g->n; v++)
        grau += g->adj[u][v];
    return grau;
}

/* ------------------------------------------------------------------ */
/* Grafo padrão (usado quando o arquivo grafo.txt não é encontrado)    */
/* Mesmo grafo que está em grafo.txt.                                  */
/* ------------------------------------------------------------------ */

static const char *VERTICES_PADRAO[] = {
    "Centro", "Rodoviaria", "Hospital", "Escola", "Mercado", "Praca",
    "Parque", "Universidade", "Aeroporto", "Shopping", "Ilha", "Farol"
};

static const char *ARESTAS_PADRAO[][2] = {
    {"Centro", "Rodoviaria"},
    {"Centro", "Hospital"},
    {"Centro", "Praca"},
    {"Rodoviaria", "Mercado"},
    {"Hospital", "Escola"},
    {"Hospital", "Praca"},
    {"Praca", "Parque"},
    {"Escola", "Universidade"},
    {"Mercado", "Universidade"},
    {"Mercado", "Shopping"},
    {"Parque", "Universidade"},
    {"Universidade", "Aeroporto"},
    {"Ilha", "Farol"}
};

void grafo_carregar_padrao(Grafo *g)
{
    int i;
    int qtd_v = (int)(sizeof(VERTICES_PADRAO) / sizeof(VERTICES_PADRAO[0]));
    int qtd_a = (int)(sizeof(ARESTAS_PADRAO) / sizeof(ARESTAS_PADRAO[0]));

    grafo_inicializar(g, 0);
    strcpy(g->titulo, "Mapa da cidade (grafo padrao)");
    for (i = 0; i < qtd_v; i++)
        grafo_adicionar_vertice(g, VERTICES_PADRAO[i]);
    for (i = 0; i < qtd_a; i++)
        grafo_adicionar_aresta(g, grafo_indice(g, ARESTAS_PADRAO[i][0]),
                                  grafo_indice(g, ARESTAS_PADRAO[i][1]));
}

/* ------------------------------------------------------------------ */
/* Leitura do grafo a partir de arquivo texto                          */
/*                                                                     */
/* Formato:                                                            */
/*   # comentario                                                      */
/*   TITULO Nome do grafo                                              */
/*   TIPO NAO_DIRECIONADO        (ou DIRECIONADO)                      */
/*   VERTICES                                                          */
/*   A                                                                 */
/*   B                                                                 */
/*   ARESTAS                                                           */
/*   A B                                                               */
/*                                                                     */
/* Retorno: 1 = ok, 0 = arquivo inválido, -1 = não abriu o arquivo.    */
/* Em caso de erro o grafo de destino não é alterado.                  */
/* ------------------------------------------------------------------ */

static void remover_espacos_finais(char *s)
{
    size_t len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1]))
        s[--len] = '\0';
}

int grafo_carregar_arquivo(Grafo *destino, const char *caminho)
{
    static Grafo g; /* static: evita colocar a matriz inteira na pilha */
    FILE *arq;
    char linha[256];
    char a[MAX_NOME], b[MAX_NOME];
    int secao = 0; /* 0 = cabeçalho, 1 = vértices, 2 = arestas */
    int num_linha = 0;

    arq = fopen(caminho, "r");
    if (arq == NULL)
        return -1;

    grafo_inicializar(&g, 0);
    strncpy(g.titulo, caminho, MAX_TITULO - 1);

    while (fgets(linha, sizeof(linha), arq) != NULL) {
        char *comentario;
        int qtd;

        num_linha++;
        comentario = strchr(linha, '#');
        if (comentario != NULL)
            *comentario = '\0';

        qtd = sscanf(linha, "%39s %39s", a, b);
        if (qtd <= 0)
            continue; /* linha vazia */

        if (nomes_iguais(a, "TITULO")) {
            char *resto = strstr(linha, a) + strlen(a);
            while (isspace((unsigned char)*resto))
                resto++;
            remover_espacos_finais(resto);
            strncpy(g.titulo, resto, MAX_TITULO - 1);
            g.titulo[MAX_TITULO - 1] = '\0';
        } else if (nomes_iguais(a, "TIPO")) {
            if (secao == 2) {
                printf("  [linha %d] TIPO deve vir antes das ARESTAS.\n", num_linha);
                fclose(arq);
                return 0;
            }
            g.direcionado = (qtd == 2 && nomes_iguais(b, "DIRECIONADO"));
        } else if (nomes_iguais(a, "VERTICES")) {
            secao = 1;
        } else if (nomes_iguais(a, "ARESTAS")) {
            secao = 2;
        } else if (secao == 1) {
            if (grafo_adicionar_vertice(&g, a) < 0)
                printf("  [linha %d] Vertice '%s' repetido ou limite de %d atingido.\n",
                       num_linha, a, MAX_VERTICES);
        } else if (secao == 2) {
            int u, v;
            if (qtd < 2) {
                printf("  [linha %d] Aresta incompleta, ignorada.\n", num_linha);
                continue;
            }
            u = grafo_indice(&g, a);
            v = grafo_indice(&g, b);
            if (u < 0 || v < 0)
                printf("  [linha %d] Aresta %s-%s usa vertice inexistente, ignorada.\n",
                       num_linha, a, b);
            else if (!grafo_adicionar_aresta(&g, u, v))
                printf("  [linha %d] Aresta %s-%s repetida, ignorada.\n", num_linha, a, b);
        } else {
            printf("  [linha %d] Linha fora de secao, ignorada.\n", num_linha);
        }
    }
    fclose(arq);

    if (g.n == 0) {
        printf("  O arquivo nao possui nenhum vertice.\n");
        return 0;
    }
    *destino = g;
    return 1;
}

/* ------------------------------------------------------------------ */
/* Exibição                                                            */
/* ------------------------------------------------------------------ */

void grafo_imprimir_resumo(const Grafo *g)
{
    printf(" Grafo: %s\n", g->titulo);
    printf(" Tipo: %s | Vertices: %d | Arestas: %d\n",
           g->direcionado ? "direcionado" : "nao direcionado", g->n, g->num_arestas);
}

void grafo_listar_vertices(const Grafo *g)
{
    int i;
    printf("\n Vertices disponiveis:\n");
    for (i = 0; i < g->n; i++) {
        printf("  [%2d] %-16s", i, g->nomes[i]);
        if ((i + 1) % 4 == 0 || i == g->n - 1)
            printf("\n");
    }
}

void grafo_imprimir_lista(const Grafo *g)
{
    int u, v, primeiro;

    printf("\n========== LISTA DE ADJACENCIA ==========\n");
    grafo_imprimir_resumo(g);
    printf("\n");
    for (u = 0; u < g->n; u++) {
        printf(" [%2d] %-16s (grau %d) %s ", u, g->nomes[u], grafo_grau(g, u),
               g->direcionado ? "->" : ":");
        primeiro = 1;
        for (v = 0; v < g->n; v++) {
            if (g->adj[u][v]) {
                printf("%s%s", primeiro ? "" : ", ", g->nomes[v]);
                primeiro = 0;
            }
        }
        if (primeiro)
            printf("(sem vizinhos)");
        printf("\n");
    }
}

void grafo_imprimir_matriz(const Grafo *g)
{
    int u, v;

    printf("\n========== MATRIZ DE ADJACENCIA ==========\n");
    grafo_imprimir_resumo(g);
    printf("\n %-21s", "");
    for (v = 0; v < g->n; v++)
        printf("%3d", v);
    printf("\n");
    for (u = 0; u < g->n; u++) {
        printf(" [%2d] %-16s", u, g->nomes[u]);
        for (v = 0; v < g->n; v++)
            printf("%3d", g->adj[u][v]);
        printf("\n");
    }
}
