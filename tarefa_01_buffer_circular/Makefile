# Variáveis de Compilação
CC = gcc
CFLAGS = -Wall -Wextra -Wconversion -std=c99 -I.

# Diretório de saída dos artefatos gerados (objetos e executável)
OUTDIR = output

# Arquivos de Origem e Destino
SRCS = buffer_circular.c testes.c
OBJS = $(addprefix $(OUTDIR)/, $(SRCS:.c=.o))
TARGET = $(OUTDIR)/testes

# Regra Principal (Default)
all: $(TARGET)

# Cria o diretório de saída, se ainda não existir
$(OUTDIR):
	mkdir -p $(OUTDIR)

# Linkagem do Executável
# O "| $(OUTDIR)" é um pré-requisito de ordem: garante que a pasta exista
# antes de compilar, sem que a data da pasta force recompilações.
$(TARGET): $(OBJS) | $(OUTDIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compilação dos Arquivos de Objeto (cada .c da raiz gera um .o em output/)
$(OUTDIR)/%.o: %.c buffer_circular.h | $(OUTDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Executa os testes unitários
run: all
	./$(TARGET)

# Limpeza dos arquivos temporários de compilação
clean:
	rm -rf $(OUTDIR)

.PHONY: all run clean
