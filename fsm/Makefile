CC     = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -O2
ALVO   = teste_fsm

ifeq ($(OS),Windows_NT)
RODA = $(ALVO)
else
RODA = ./$(ALVO)
endif

test: $(ALVO)
	$(RODA)

$(ALVO): protocolo.c test_protocolo.c protocolo.h
	$(CC) $(CFLAGS) -o $@ protocolo.c test_protocolo.c
