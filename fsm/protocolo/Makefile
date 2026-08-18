# Makefile - testes das FSMs do protocolo
#
#   make          compila os testes e o demo
#   make test     compila e executa os testes
#   make clean    remove os arquivos gerados

CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -pedantic
FSM     = protocolo_tx.c protocolo_rx.c

all: testes demo

testes: testes.c $(FSM) protocolo.h
	$(CC) $(CFLAGS) -o testes testes.c $(FSM)

demo: main.c $(FSM) protocolo.h
	$(CC) $(CFLAGS) -o demo main.c $(FSM)

test: testes
	./testes

clean:
	-rm -f testes demo testes.exe demo.exe *.o

.PHONY: all test clean
