# Makefile - testes do modulo de buffer circular
#
#   make          compila os testes
#   make test     compila e executa os testes
#   make clean    remove os arquivos gerados

CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -pedantic
ALVO    = testes
FONTES  = testes_buffer_circular.c buffer_circular.c

all: $(ALVO)

$(ALVO): $(FONTES) buffer_circular.h
	$(CC) $(CFLAGS) -o $(ALVO) $(FONTES)

test: $(ALVO)
	./$(ALVO)

clean:
	-rm -f $(ALVO) $(ALVO).exe *.o

.PHONY: all test clean
