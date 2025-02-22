#include "functions.h"

#define MAX_VERTICES 1000  // Ajuste conforme necessário

// Função para criar a matriz de adjacência
void criar_matriz(int num_vertices, int arestas[][2], int num_arestas, int matriz_adj[][MAX_VERTICES]) {
    for (int i = 0; i < num_arestas; i++) {
        int u = arestas[i][0];
        int v = arestas[i][1];
        matriz_adj[u][v] = 1;
        matriz_adj[v][u] = 1;
    }
}

// Função que calcula os vizinhos comuns
int vizinhos(int u, int v, int num_vertices, int matriz_adj[][MAX_VERTICES]) {
    int count = 0;
    for (int i = 0; i < num_vertices; i++) {
        if (matriz_adj[u][i] && matriz_adj[v][i]) {
            count++;
        }
    }
    return count;
}

// Função para ler o grafo de um arquivo
void ler_grafico(const char *nome_arquivo, int arestas[][2], int *num_vertices, int *num_arestas) {
    FILE *arq = fopen(nome_arquivo, "r");
    if (!arq) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }
    *num_arestas = 0;
    while (fscanf(arq, "%d %d", &arestas[*num_arestas][0], &arestas[*num_arestas][1]) != EOF) {
        *num_arestas += 1;
    }
    fclose(arq);

    // Determina o número de vértices
    *num_vertices = 0;
    for (int i = 0; i < *num_arestas; i++) {
        if (arestas[i][0] > *num_vertices) *num_vertices = arestas[i][0];
        if (arestas[i][1] > *num_vertices) *num_vertices = arestas[i][1];
    }
    *num_vertices += 1;  // Ajustar para indexação zero
}


// Função para modificar o nome do arquivo de entrada para .cng
void extensao(char *nome_arquivo) {
    char *dot = strrchr(nome_arquivo, '.');
    if (dot) *dot = '\0';  // Remove a extensão anterior, se existir
    strcat(nome_arquivo, ".cng");  // Adiciona a extensão .cng
}

