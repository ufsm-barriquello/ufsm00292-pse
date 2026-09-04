CC     = gcc
CFLAGS = -std=c99 -Wall -Wextra -Wno-implicit-fallthrough
DEMO   = protothreads
TESTES = teste_protothreads

ifeq ($(OS),Windows_NT)
RODA = 
else
RODA = ./
endif

test: $(TESTES)
	$(RODA)$(TESTES)

run: $(DEMO)
	$(RODA)$(DEMO)

$(DEMO): main.c protocolo_pt.c protocolo_pt.h pt.h
	$(CC) $(CFLAGS) -o $@ main.c protocolo_pt.c

$(TESTES): test_protocolo_pt.c protocolo_pt.c protocolo_pt.h pt.h
	$(CC) $(CFLAGS) -o $@ test_protocolo_pt.c protocolo_pt.c
