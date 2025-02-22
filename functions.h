#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <omp.h>


#define MAX_VERTICES 1000 // Defina o valor máximo conforme a necessidade

// Função para criar a matriz de adjacência
void criar_matriz(int num_vertices, int arestas[][2], int num_arestas, int matriz_adj[][MAX_VERTICES]);

// Função que calcula os vizinhos comuns
int vizinhos(int u, int v, int num_vertices, int matriz_adj[][MAX_VERTICES]);

// Função para ler o grafo de um arquivo
void ler_grafico(const char *nome_arquivo, int arestas[][2], int *num_vertices, int *num_arestas);

// Função para modificar o nome do arquivo
void extensao(char *nome_arquivo);