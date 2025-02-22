# Nome do compilador
CC = mpicc

# Flags
CFLAGS = -Wall -O2 -fopenmp

# Nome do executável
TARGET = main
# Arquivos fonte e cabeçalho
SRCS = main_MPI_OPM.c functions.c
HDRS = functions.h

# Regra padrão para compilar o projeto
$(TARGET): $(SRCS) $(HDRS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

# Regra para limpar os arquivos compilados
clean:
	rm -f $(TARGET)


