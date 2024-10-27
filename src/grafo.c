#include "../include/grafo.h"
#include <stdlib.h>
#include <stdio.h>

Grafo *criarGrafo(int numVertices, TipoRepresentacao tipo) {
    Grafo *grafo = (Grafo *)malloc(sizeof(Grafo));
    grafo->numVertices = numVertices;
    grafo->numArestas = 0;
    grafo->tipo = tipo;

    if (tipo == MATRIZ_ADJACENCIA) {
        grafo->grafoMatriz = criarGrafoMatriz(numVertices);
        grafo->grafoLista = NULL;
    } else if (tipo == LISTA_ADJACENCIA) {
        grafo->grafoLista = criarGrafoLista(numVertices);
        grafo->grafoMatriz = NULL;
    }
    return grafo;
}

// Função para ler as arestas do arquivo e preencher o grafo
void lerArestas(Grafo *grafo, const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }

    int numVertices;
    fscanf(arquivo, "%d", &numVertices);  // Ignora o número de vértices, já foi lido

    int u, v;
    while (fscanf(arquivo, "%d %d", &u, &v) != EOF) {
        // Verifica se os índices dos vértices são válidos
        if (u <= 0 || u > grafo->numVertices || v <= 0 || v > grafo->numVertices) {
            printf("Vértice inválido: %d ou %d\n", u, v);
            continue; // Ignora esta aresta inválida
        }

        // Adiciona aresta na matriz de adjacência ou lista de adjacência
        if (grafo->tipo == MATRIZ_ADJACENCIA) {
            grafo->grafoMatriz->matriz[u - 1][v - 1] = 1;
            grafo->grafoMatriz->matriz[v - 1][u - 1] = 1;
        } else if (grafo->tipo == LISTA_ADJACENCIA) {
            No *novoNoU = (No *)malloc(sizeof(No));
            novoNoU->vertice = v - 1;
            novoNoU->prox = grafo->grafoLista->listaAdj[u - 1];
            grafo->grafoLista->listaAdj[u - 1] = novoNoU;

            No *novoNoV = (No *)malloc(sizeof(No));
            novoNoV->vertice = u - 1;
            novoNoV->prox = grafo->grafoLista->listaAdj[v - 1];
            grafo->grafoLista->listaAdj[v - 1] = novoNoV;
        }
    }

    fclose(arquivo);
}


void liberarGrafo(Grafo *grafo) {
    if (grafo->tipo == MATRIZ_ADJACENCIA) {
        liberarGrafoMatriz(grafo->grafoMatriz);
    } else if (grafo->tipo == LISTA_ADJACENCIA) {
        liberarGrafoLista(grafo->grafoLista);
    }
    free(grafo);
}

// Função auxiliar para calcular estatísticas dos graus
EstatisticasGrafo *calcularEstatisticasGrafo(Grafo *grafo) {
    EstatisticasGrafo *stats = (EstatisticasGrafo *)malloc(sizeof(EstatisticasGrafo));
    int *graus = (int *)malloc(grafo->numVertices * sizeof(int));
    int totalGrau = 0;

    // Inicializar valores
    stats->grauMinimo = grafo->numVertices;
    stats->grauMaximo = 0;
    stats->numArestas = 0;

    // Calcular o grau de cada vértice
    for (int i = 0; i < grafo->numVertices; i++) {
        int grau = 0;
        if (grafo->tipo == MATRIZ_ADJACENCIA) {
            for (int j = 0; j < grafo->numVertices; j++) {
                grau += grafo->grafoMatriz->matriz[i][j];
            }
        } else if (grafo->tipo == LISTA_ADJACENCIA) {
            No *atual = grafo->grafoLista->listaAdj[i];
            while (atual != NULL) {
                grau++;
                atual = atual->prox;
            }
        }

        graus[i] = grau;
        stats->numArestas += grau;

        // Atualizar grau mínimo e máximo
        if (grau < stats->grauMinimo) stats->grauMinimo = grau;
        if (grau > stats->grauMaximo) stats->grauMaximo = grau;
        totalGrau += grau;
    }

    // Calcular grau médio
    stats->grauMedio = (float)totalGrau / grafo->numVertices;

    // Calcular mediana
    qsort(graus, grafo->numVertices, sizeof(int), compararInteiros);
    if (grafo->numVertices % 2 == 0) {
        stats->medianaGrau = (graus[grafo->numVertices / 2 - 1] + graus[grafo->numVertices / 2]) / 2;
    } else {
        stats->medianaGrau = graus[grafo->numVertices / 2];
    }

    // Dividir por 2 pois cada aresta foi contada duas vezes
    stats->numArestas /= 2;
    stats->numVertices = grafo->numVertices;

    // Limpar
    free(graus);

    return stats;
}

// Função auxiliar para comparação
int compararInteiros(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

// Cria uma pilha com capacidade específica
Pilha *criarPilha(int capacidade) {
    Pilha *pilha = (Pilha *)malloc(sizeof(Pilha));
    pilha->dados = (int *)malloc(capacidade * sizeof(int));
    pilha->topo = -1;
    pilha->capacidade = capacidade;
    return pilha;
}

// Empilha um valor na pilha
void push(Pilha *pilha, int valor) {
    if (pilha->topo < pilha->capacidade - 1) {
        pilha->dados[++pilha->topo] = valor;
    }
}

// Desempilha um valor da pilha
int pop(Pilha *pilha) {
    if (pilha->topo >= 0) {
        return pilha->dados[pilha->topo--];
    }
    return -1; // Indica que a pilha está vazia
}

// Verifica se a pilha está vazia
int estaVaziaPilha(Pilha *pilha) {
    return pilha->topo == -1;
}

// Libera a memória da pilha
void liberarPilha(Pilha *pilha) {
    free(pilha->dados);
    free(pilha);
}

// Função para criar um grafo vazio (usado para a árvore DFS)
Grafo *criarGrafoVazio(int numVertices, TipoRepresentacao tipo) {
    Grafo *grafo = (Grafo *)malloc(sizeof(Grafo));
    grafo->numVertices = numVertices;
    grafo->numArestas = 0;
    grafo->tipo = tipo;

    if (tipo == MATRIZ_ADJACENCIA) {
        grafo->grafoMatriz = criarGrafoMatriz(numVertices);
        grafo->grafoLista = NULL;
    } else if (tipo == LISTA_ADJACENCIA) {
        grafo->grafoLista = criarGrafoLista(numVertices);
        grafo->grafoMatriz = NULL;
    }
    return grafo;
}

// Função para adicionar uma aresta ao grafo (usada para construir a árvore DFS)
void adicionarArestaGrafo(Grafo *grafo, int u, int v) {
    if (grafo->tipo == MATRIZ_ADJACENCIA) {
        adicionarArestaMatriz(grafo->grafoMatriz, u, v);
    } else if (grafo->tipo == LISTA_ADJACENCIA) {
        adicionarArestaLista(grafo->grafoLista, u, v);
    }
    grafo->numArestas++;
}

// Função principal para DFS com pilha que constrói a árvore DFS
void dfsComPilhaArvore(Grafo *grafo, int verticeInicial, int *visitados, Grafo *arvoreDFS, int *pais, int *niveis, FILE *arquivoSaida) {
    Pilha *pilha = criarPilha(grafo->numVertices);
    push(pilha, verticeInicial);

    pais[verticeInicial] = verticeInicial; // O pai do vértice inicial é ele mesmo
    niveis[verticeInicial] = 0;

    while (!estaVaziaPilha(pilha)) {
        int verticeAtual = pop(pilha);

        if (!visitados[verticeAtual]) {
            visitados[verticeAtual] = 1;

            // Imprime no arquivo de saída
            fprintf(arquivoSaida, "Vértice: %d, Pai: %d, Nível: %d\n", verticeAtual + 1, pais[verticeAtual] + 1, niveis[verticeAtual]);

            // Adiciona aresta ao grafo árvore, se não for o vértice inicial
            if (verticeAtual != verticeInicial) {
                adicionarArestaGrafo(arvoreDFS, verticeAtual, pais[verticeAtual]);
            }

            // Explora os vértices adjacentes
            if (grafo->tipo == MATRIZ_ADJACENCIA) {
                for (int j = grafo->numVertices - 1; j >= 0; j--) {
                    if (grafo->grafoMatriz->matriz[verticeAtual][j] == 1 && !visitados[j]) {
                        push(pilha, j);
                        pais[j] = verticeAtual;
                        niveis[j] = niveis[verticeAtual] + 1;
                    }
                }
            } else if (grafo->tipo == LISTA_ADJACENCIA) {
                No *atual = grafo->grafoLista->listaAdj[verticeAtual];
                while (atual != NULL) {
                    int v = atual->vertice;
                    if (!visitados[v]) {
                        push(pilha, v);
                        pais[v] = verticeAtual;
                        niveis[v] = niveis[verticeAtual] + 1;
                    }
                    atual = atual->prox;
                }
            }
        }
    }

    liberarPilha(pilha);
}

// Função para criar uma fila com capacidade específica
Fila *criarFila(int capacidade) {
    Fila *fila = (Fila *)malloc(sizeof(Fila));
    fila->dados = (int *)malloc(capacidade * sizeof(int));
    fila->frente = -1;  // Inicialize com -1 para indicar que a fila está vazia
    fila->tras = -1;    // Inicialize com -1 para indicar que a fila está vazia
    fila->capacidade = capacidade;
    return fila;
}

// Enfileira um valor na fila
void enqueue(Fila *fila, int valor) {
    // Verifica se a fila está cheia
    if ((fila->tras + 1) % fila->capacidade == fila->frente) {
        printf("Erro: Fila cheia ao tentar enfileirar %d\n", valor + 1);
        return;
    }

    if (fila->frente == -1) {
        // Caso especial para o primeiro elemento
        fila->frente = 0;
    }

    fila->tras = (fila->tras + 1) % fila->capacidade;
    fila->dados[fila->tras] = valor;
}

// Desenfileira um valor da fila
int dequeue(Fila *fila) {
    if (fila->frente == -1) {
        // Fila vazia
        printf("Erro: Fila vazia ao tentar desenfileirar\n");
        return -1;
    }

    int valor = fila->dados[fila->frente];
    if (fila->frente == fila->tras) {
        // Fila ficará vazia após esta operação
        fila->frente = -1;
        fila->tras = -1;
    } else {
        fila->frente = (fila->frente + 1) % fila->capacidade;
    }

    return valor;
}

// Verifica se a fila está vazia
int estaVaziaFila(Fila *fila) {
    int vazia = (fila->frente == -1 && fila->tras == -1);
    return vazia;
}

// Libera a memória da fila
void liberarFila(Fila *fila) {
    free(fila->dados);
    free(fila);
}

// Função principal para BFS
void bfsComFilaArvore(Grafo *grafo, int verticeInicial, int *visitados, Grafo *arvoreBFS, int *pais, int *niveis, FILE *arquivoSaida) {
    Fila *fila = criarFila(grafo->numVertices);
    enqueue(fila, verticeInicial);
    visitados[verticeInicial] = 1;
    pais[verticeInicial] = verticeInicial; // The parent of the initial vertex is itself
    niveis[verticeInicial] = 0;

    while (!estaVaziaFila(fila)) {
        int verticeAtual = dequeue(fila);

        // Write to the output file
        fprintf(arquivoSaida, "Vértice: %d, Pai: %d, Nível: %d\n", verticeAtual + 1, pais[verticeAtual] + 1, niveis[verticeAtual]);

        // Add edge to the BFS tree graph, if not the initial vertex
        if (verticeAtual != verticeInicial) {
            adicionarArestaGrafo(arvoreBFS, verticeAtual, pais[verticeAtual]);
        }

        // Explore adjacent vertices
        if (grafo->tipo == MATRIZ_ADJACENCIA) {
            for (int j = 0; j < grafo->numVertices; j++) {
                if (grafo->grafoMatriz->matriz[verticeAtual][j] == 1 && !visitados[j]) {
                    visitados[j] = 1;  // Mark as visited
                    enqueue(fila, j);   // Enqueue the adjacent vertex
                    pais[j] = verticeAtual;
                    niveis[j] = niveis[verticeAtual] + 1;
                }
            }
        } else if (grafo->tipo == LISTA_ADJACENCIA) {
            No *atual = grafo->grafoLista->listaAdj[verticeAtual];
            while (atual != NULL) {
                int v = atual->vertice;
                if (!visitados[v]) {
                    visitados[v] = 1;  // Mark as visited
                    enqueue(fila, v);   // Enqueue the adjacent vertex
                    pais[v] = verticeAtual;
                    niveis[v] = niveis[verticeAtual] + 1;
                }
                atual = atual->prox;
            }
        }
    }

    liberarFila(fila);
}

