/*
 * ATIVIDADE AVALIATIVA 3 - Implementacao de grafos em C
 *
 * Menu interativo com:
 *   - Busca em profundidade (DFS) recursiva com backtracking
 *   - Busca em largura (BFS) com fila
 *   - Caminho percorrido e dados gerais de cada busca
 *
 * Compilar:  gcc -Wall -Wextra -std=c99 -o grafo main.c grafo.c fila.c busca.c
 * Executar:  ./grafo              (le o grafo de grafo.txt)
 *            ./grafo outro.txt    (le o grafo de outro arquivo)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "grafo.h"
#include "busca.h"

#define ARQUIVO_PADRAO "grafo.txt"

/* ------------------------------------------------------------------ */
/* Entrada do usuario                                                  */
/* ------------------------------------------------------------------ */

/* Le uma linha inteira do teclado, sem o '\n' e sem espacos nas pontas. */
static void ler_linha(char *buf, int tam)
{
    char *inicio;
    size_t len;

    if (fgets(buf, tam, stdin) == NULL) {
        printf("\nEntrada encerrada. Saindo...\n");
        exit(0);
    }
    len = strlen(buf);
    while (len > 0 && isspace((unsigned char)buf[len - 1]))
        buf[--len] = '\0';
    inicio = buf;
    while (isspace((unsigned char)*inicio))
        inicio++;
    memmove(buf, inicio, strlen(inicio) + 1);
}

static void pausar(void)
{
    char buf[8];
    printf("\nPressione ENTER para continuar...");
    ler_linha(buf, sizeof(buf));
}

/*
 * Pede um vertice ao usuario, que pode digitar o NOME ou o NUMERO.
 * Se 'opcional' for 1, ENTER vazio retorna -1 (sem vertice).
 */
static int ler_vertice(const Grafo *g, const char *mensagem, int opcional)
{
    char buf[100];
    char *fim;
    long numero;
    int indice;

    for (;;) {
        printf("%s", mensagem);
        ler_linha(buf, sizeof(buf));

        if (buf[0] == '\0') {
            if (opcional)
                return -1;
            printf("  Informe um vertice.\n");
            continue;
        }

        /* Primeiro tenta pelo nome (funciona mesmo se os nomes forem numeros). */
        indice = grafo_indice(g, buf);
        if (indice >= 0)
            return indice;

        numero = strtol(buf, &fim, 10);
        if (*fim == '\0' && numero >= 0 && numero < g->n)
            return (int)numero;

        printf("  Vertice invalido. Digite o numero [0-%d] ou o nome.\n", g->n - 1);
    }
}

/* ------------------------------------------------------------------ */
/* Acoes do menu                                                       */
/* ------------------------------------------------------------------ */

static void executar_busca(const Grafo *g, int tipo, int passo_a_passo)
{
    static ResultadoBusca resultado;
    int origem, objetivo;

    printf("\n===== %s =====\n",
           tipo == BUSCA_DFS ? "BUSCA EM PROFUNDIDADE (DFS)" : "BUSCA EM LARGURA (BFS)");
    grafo_listar_vertices(g);
    printf("\n");
    origem = ler_vertice(g, " Vertice de origem: ", 0);
    objetivo = ler_vertice(g, " Vertice objetivo (ENTER = percorrer o grafo todo): ", 1);

    if (tipo == BUSCA_DFS)
        busca_profundidade(g, origem, objetivo, passo_a_passo, &resultado);
    else
        busca_largura(g, origem, objetivo, passo_a_passo, &resultado);

    busca_imprimir_resultado(g, &resultado);
}

static void executar_comparacao(const Grafo *g)
{
    int origem, objetivo;

    printf("\n===== COMPARAR DFS x BFS =====\n");
    grafo_listar_vertices(g);
    printf("\n");
    origem = ler_vertice(g, " Vertice de origem: ", 0);
    objetivo = ler_vertice(g, " Vertice objetivo (ENTER = percorrer o grafo todo): ", 1);
    busca_comparar(g, origem, objetivo);
}

static void carregar_de_arquivo(Grafo *g)
{
    char caminho[256];
    int status;

    printf("\n Nome do arquivo (ENTER = %s): ", ARQUIVO_PADRAO);
    ler_linha(caminho, sizeof(caminho));
    if (caminho[0] == '\0')
        strcpy(caminho, ARQUIVO_PADRAO);

    status = grafo_carregar_arquivo(g, caminho);
    if (status == 1) {
        printf(" Grafo carregado de '%s'.\n", caminho);
        grafo_imprimir_resumo(g);
    } else if (status == -1) {
        printf(" Nao foi possivel abrir '%s'. O grafo atual foi mantido.\n", caminho);
    } else {
        printf(" Arquivo '%s' invalido. O grafo atual foi mantido.\n", caminho);
    }
}

static void exibir_menu(const Grafo *g, int passo_a_passo)
{
    printf("\n==========================================================\n");
    printf("      GRAFOS EM C - BUSCA EM PROFUNDIDADE E EM LARGURA\n");
    printf("==========================================================\n");
    grafo_imprimir_resumo(g);
    printf(" Modo passo a passo: %s\n", passo_a_passo ? "LIGADO" : "DESLIGADO");
    printf("----------------------------------------------------------\n");
    printf("  1 - Exibir grafo (lista de adjacencia)\n");
    printf("  2 - Exibir matriz de adjacencia\n");
    printf("  3 - Busca em profundidade (DFS)\n");
    printf("  4 - Busca em largura (BFS)\n");
    printf("  5 - Comparar DFS x BFS\n");
    printf("  6 - Ligar/desligar modo passo a passo\n");
    printf("  7 - Carregar grafo de arquivo\n");
    printf("  8 - Usar grafo padrao (embutido no programa)\n");
    printf("  0 - Sair\n");
    printf("----------------------------------------------------------\n");
    printf(" Escolha uma opcao: ");
}

/* ------------------------------------------------------------------ */
/* Programa principal                                                  */
/* ------------------------------------------------------------------ */

int main(int argc, char *argv[])
{
    static Grafo grafo;
    const char *arquivo = (argc > 1) ? argv[1] : ARQUIVO_PADRAO;
    int passo_a_passo = 1;
    char opcao[16];
    int status;

    status = grafo_carregar_arquivo(&grafo, arquivo);
    if (status == 1) {
        printf("Grafo carregado de '%s'.\n", arquivo);
    } else {
        printf("Nao foi possivel usar '%s'. Carregando o grafo padrao.\n", arquivo);
        grafo_carregar_padrao(&grafo);
    }

    for (;;) {
        exibir_menu(&grafo, passo_a_passo);
        ler_linha(opcao, sizeof(opcao));

        if (strcmp(opcao, "0") == 0) {
            printf("\nEncerrando o programa. Ate mais!\n");
            break;
        } else if (strcmp(opcao, "1") == 0) {
            grafo_imprimir_lista(&grafo);
        } else if (strcmp(opcao, "2") == 0) {
            grafo_imprimir_matriz(&grafo);
        } else if (strcmp(opcao, "3") == 0) {
            executar_busca(&grafo, BUSCA_DFS, passo_a_passo);
        } else if (strcmp(opcao, "4") == 0) {
            executar_busca(&grafo, BUSCA_BFS, passo_a_passo);
        } else if (strcmp(opcao, "5") == 0) {
            executar_comparacao(&grafo);
        } else if (strcmp(opcao, "6") == 0) {
            passo_a_passo = !passo_a_passo;
            printf("\n Modo passo a passo %s.\n", passo_a_passo ? "LIGADO" : "DESLIGADO");
        } else if (strcmp(opcao, "7") == 0) {
            carregar_de_arquivo(&grafo);
        } else if (strcmp(opcao, "8") == 0) {
            grafo_carregar_padrao(&grafo);
            printf("\n Grafo padrao carregado.\n");
        } else {
            printf("\n Opcao invalida. Digite um numero de 0 a 8.\n");
            continue;
        }
        pausar();
    }
    return 0;
}
