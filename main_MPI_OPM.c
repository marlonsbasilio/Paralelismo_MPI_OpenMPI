#include "functions.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <arquivo_edgelist>\n", argv[0]);
        return 1;
    }

    int rank, size;
    MPI_Init(&argc, &argv);  // Inicializa o MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Rank do processo
    MPI_Comm_size(MPI_COMM_WORLD, &size);  // Número de processos MPI

    int arestas[MAX_VERTICES][2];
    int num_vertices, num_arestas;
    int matriz_adj[MAX_VERTICES][MAX_VERTICES] = {0};

    char nome_arquivo[256];
    strcpy(nome_arquivo, argv[1]);
    extensao(nome_arquivo);

    // Apenas o processo 0 mede o tempo total do programa
    double t_inicial = 0.0, t_escrita = 0.0, calc_t = 0.0, t_leitura = 0.0;
    if (rank == 0){ 
        t_inicial = MPI_Wtime();
    }

     // Leitura do arquivo (medida apenas pelo processo 0)
    double t_escrita_inicial = MPI_Wtime();
    if (rank == 0) {
        ler_grafico(argv[1], arestas, &num_vertices, &num_arestas);
        criar_matriz(num_vertices, arestas, num_arestas, matriz_adj);
    }
    if (rank == 0){
        t_escrita = MPI_Wtime() - t_escrita_inicial;
    }

    // Distribui os dados para todos os processos
    MPI_Bcast(&num_vertices, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&num_arestas, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(matriz_adj, MAX_VERTICES * MAX_VERTICES, MPI_INT, 0, MPI_COMM_WORLD);

    // Sincroniza os processos antes de iniciar os cálculos
    MPI_Barrier(MPI_COMM_WORLD);
    double calc_t_inicial = MPI_Wtime();

    // Cada processo MPI processa um subconjunto dos vértices, mas agora usa OpenMP dentro desse bloco
    char buffer[1024 * 10];
    int buffer_len = 0;

    #pragma omp parallel
    {

        char local_buffer[1024 * 10]; // Buffer local por thread
        int local_buffer_len = 0;
        
        #pragma omp single
        for (int u = rank; u < num_vertices; u += size) {
            for (int v = u + 1; v < num_vertices; v++) {
                #pragma omp task firstprivate(u, v)
                {
                    int v_comuns = vizinhos(u, v, num_vertices, matriz_adj);
                    if (v_comuns > 0) {
                        local_buffer_len += sprintf(local_buffer + local_buffer_len, "%d %d %d\n", u, v, v_comuns);
                    }
                }
            }

            // Região crítica para consolidar os dados no buffer global
            #pragma omp critical
            {
                memcpy(buffer + buffer_len, local_buffer, local_buffer_len);
                buffer_len += local_buffer_len;
            }
        }
    }

    calc_t = MPI_Wtime() - calc_t_inicial;  // Fim da medição de tempo de cálculo para cada processo

    // Sincroniza todos os processos
    MPI_Barrier(MPI_COMM_WORLD);
    double t_leitura_inicial = MPI_Wtime();

    // O processo 0 coleta os dados e escreve no arquivo
    if (rank == 0) {
        FILE *arq_saida = fopen("output.cng", "w");
        fwrite(buffer, sizeof(char), buffer_len, arq_saida);
        fclose(arq_saida);

        for (int i = 1; i < size; i++) {
            MPI_Recv(&buffer_len, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(buffer, buffer_len, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            arq_saida = fopen("output.cng", "a");
            fwrite(buffer, sizeof(char), buffer_len, arq_saida);
            fclose(arq_saida);
        }
    } else {
        MPI_Send(&buffer_len, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(buffer, buffer_len, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }

    if (rank == 0){ 
        t_leitura = MPI_Wtime() - t_leitura_inicial;  // Tempo de escrita apenas para processo 0
    }

    if (rank == 0) {
        double t_total = MPI_Wtime() - t_inicial;
        printf("\n=== Tempos de Execução (Processo 0) ===\n");
        printf("Tempo total do programa: %f segundos\n", t_total);
        printf("Tempo de leitura do arquivo: %f segundos\n", t_escrita);
        printf("Tempo de escrita do arquivo .cng): %f segundos\n", t_leitura);
        remove(argv[1]);
    }

    // Relatório de tempo de cálculo para cada processo
    printf("Processo %d: Tempo de cálculo dos vizinhos: %f segundos\n", rank, calc_t);
    if (rank == 0) {
        printf("\n");
    }

    MPI_Finalize();
    return 0;
}