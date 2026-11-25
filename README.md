# AVALIATIVA 3 — Grafos em C

Programa em C com menu que faz busca em profundidade (DFS) e busca em largura (BFS) em um grafo.

## Integrantes

- Giovanni Beker — RA a2767007

## Como rodar

```bash
gcc -o grafo main.c
./grafo        # no Windows: grafo.exe
```

## O grafo

Grafo não direcionado com 10 vértices, guardado em uma matriz de adjacência. Ele é montado na função `montarGrafo()` do [main.c](main.c). Para trocar pelo grafo da Atividade 2, mude o vetor `nomes`, o valor de `N` e as chamadas de `adicionarAresta()`.

```
Centro - Rodoviaria, Hospital, Praca
Rodoviaria - Mercado
Hospital - Escola, Praca
Escola - Universidade
Mercado - Universidade
Praca - Parque
Parque - Universidade
Ilha - Farol      (separados do resto: testam o "objetivo nao encontrado")
```

## Requisitos

| Requisito | Onde está no `main.c` |
|---|---|
| Menu interativo | `main()` |
| DFS com recursividade e backtracking | `dfs()`: chama a si mesma e, quando acabam os vizinhos, volta para o vértice anterior |
| BFS com fila | `bfs()`: usa o vetor `fila` com `inicio` e `fim` |
| Caminho percorrido | `mostrarResultado()` |
| Dados gerais da busca | `mostrarResultado()`: origem, vértices visitados, quantidade de vértices, quantidade de arestas percorridas, caminho e se o objetivo foi encontrado |
