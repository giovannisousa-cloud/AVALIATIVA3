# AVALIATIVA 3 — Implementação de grafos em C

Programa em C com menu interativo que carrega um grafo e executa **busca em profundidade (DFS)** e **busca em largura (BFS)**. Para cada busca ele mostra o caminho percorrido e os dados gerais da execução.

## Integrantes

- _(nome — RA)_
- _(nome — RA)_
- _(nome — RA)_

## Como compilar e executar

Com o GCC (MinGW no Windows, ou Linux/macOS):

```bash
gcc -Wall -Wextra -std=c99 -o grafo main.c grafo.c fila.c busca.c
./grafo            # Windows: grafo.exe
```

Ou com `make`:

```bash
make
make executar
```

No **Dev-C++ / Code::Blocks**, crie um projeto de console em C e adicione os quatro arquivos `.c` e os três `.h`. Deixe o `grafo.txt` na mesma pasta do executável.

O programa lê o grafo de `grafo.txt`. Para usar outro arquivo, passe o nome dele: `./grafo outro.txt`. Se o arquivo não for encontrado, o programa usa um grafo padrão que vem embutido no código.

## O grafo (Atividade 2)

O grafo fica em [`grafo.txt`](grafo.txt). Para usar o grafo da Atividade 2, basta editar esse arquivo, sem mexer no código:

```text
TITULO Nome do grafo
TIPO NAO_DIRECIONADO        # ou DIRECIONADO

VERTICES
A
B
C

ARESTAS
A B
B C
```

- Linhas que começam com `#` são comentários.
- Os nomes dos vértices não podem ter espaços (use `_`, por exemplo `Ponto_A`).
- O `TIPO` precisa vir antes de `ARESTAS`.
- O limite é de 50 vértices.

O grafo de exemplo é um mapa de cidade com 12 vértices e 13 arestas, não direcionado. `Ilha` e `Farol` formam um componente separado do resto, o que permite mostrar o caso em que o objetivo **não é encontrado** (por exemplo, origem `Centro` e objetivo `Farol`).

## Menu

```
  1 - Exibir grafo (lista de adjacencia)
  2 - Exibir matriz de adjacencia
  3 - Busca em profundidade (DFS)
  4 - Busca em largura (BFS)
  5 - Comparar DFS x BFS
  6 - Ligar/desligar modo passo a passo
  7 - Carregar grafo de arquivo
  8 - Usar grafo padrao (embutido no programa)
  0 - Sair
```

Nas buscas, o vértice pode ser informado pelo **nome** ou pelo **número**. O objetivo é opcional: se você apertar ENTER sem digitar nada, a busca percorre tudo o que dá para alcançar a partir da origem.

Com o **modo passo a passo** ligado (é o padrão), o programa mostra cada decisão do algoritmo: qual aresta foi analisada, quando avança, quando um vértice já tinha sido visitado, quando há backtracking (DFS) e como fica a fila depois de cada passo (BFS).

## Onde está cada requisito

| Requisito | Onde está |
|---|---|
| Menu interativo | `main.c` → `exibir_menu()` e o laço em `main()` |
| DFS recursiva com backtracking | `busca.c` → `dfs_recursiva()` / `busca_profundidade()` |
| BFS com estrutura de fila | `busca.c` → `busca_largura()`; a fila circular está em `fila.c` / `fila.h` |
| Caminho percorrido | `busca.c` → `busca_imprimir_resultado()`: ordem de visita, percurso com backtracking (DFS), visita por níveis (BFS) e tabela de visita |
| Dados gerais da busca | `busca.c` → `busca_imprimir_resultado()` |

Dados mostrados ao final de cada busca:

- vértice de origem;
- vértice objetivo e se ele foi **ENCONTRADO** ou **NÃO ENCONTRADO** (quando um objetivo é informado);
- vértices visitados e a quantidade deles;
- **arestas percorridas**: as que levaram a um vértice novo, ou seja, as arestas da árvore de busca;
- **arestas analisadas**: todas as arestas verificadas, inclusive as que apontavam para vértices já visitados;
- retrocessos por backtracking (somente na DFS);
- caminho percorrido, na ordem de visita;
- caminho da origem até o objetivo, quando ele é encontrado (na BFS esse é o menor caminho em número de arestas);
- vértices que não foram alcançados a partir da origem.

## Como os algoritmos funcionam

**DFS (`dfs_recursiva`)**: marca o vértice atual como visitado e, para cada vizinho ainda não visitado, chama a si mesma recursivamente. Quando um vértice não tem mais vizinhos novos, a chamada termina e a execução volta ao vértice anterior (**backtracking**), que continua testando os próximos vizinhos. Se o objetivo for encontrado, a função retorna `1` e todas as chamadas são encerradas.

**BFS (`busca_largura`)**: coloca a origem na fila. Enquanto a fila não estiver vazia, retira o primeiro vértice, visita esse vértice e coloca no fim da fila todos os vizinhos que ainda não foram descobertos. Com isso, os vértices são visitados por níveis de distância a partir da origem.

Os vizinhos são sempre testados na ordem dos índices (a ordem em que aparecem em `VERTICES`), então o resultado é sempre o mesmo para a mesma entrada.

### Exemplo (grafo padrão, origem `Centro`, sem objetivo)

```
DFS: Centro -> Rodoviaria -> Mercado -> Universidade -> Escola -> Hospital -> Praca -> Parque -> Aeroporto -> Shopping
BFS: Centro -> Rodoviaria -> Hospital -> Praca -> Mercado -> Escola -> Parque -> Universidade -> Shopping -> Aeroporto
```

Nas duas buscas, 10 dos 12 vértices são visitados; `Ilha` e `Farol` não são alcançados.

## Estrutura dos arquivos

```
main.c     menu e leitura do teclado
grafo.c/h  estrutura do grafo (matriz de adjacência), leitura do arquivo e exibição
fila.c/h   fila circular usada pela BFS
busca.c/h  DFS, BFS, impressão dos resultados e comparação
grafo.txt  grafo usado pelo programa
Makefile   atalho para compilar
```
