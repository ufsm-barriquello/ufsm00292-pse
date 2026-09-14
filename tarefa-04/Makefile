CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I.

# Diretorio para arquivos gerados (objetos e executavel)
BUILD_DIR = output

# Arquivos fonte do projeto (exemplo baseado na Tarefa-04)
SRCS = pt_protocolo.c testes_pt.c

# Mapeia os arquivos .c para .o dentro do diretorio build/
OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCS))
TARGET = $(BUILD_DIR)/testes_pt

all: $(TARGET)

# Regra para criar o diretorio build/ caso nao exista
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Regra de compilacao dos arquivos objeto (.o) dentro de build/
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Regra para linkar o executavel final dentro de build/
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# Compila e executa o projeto
run: $(TARGET)
	./$(TARGET)

# Limpa toda a pasta de arquivos gerados de uma so vez
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all run clean
