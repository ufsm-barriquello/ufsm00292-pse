CC = gcc
CFLAGS = -Wall -Wextra -std=c99
OUTPUT_DIR = output
TARGET = $(OUTPUT_DIR)/testes_fsm_tbl
OBJECTS = $(OUTPUT_DIR)/fsm_tbl_protocolo.o $(OUTPUT_DIR)/testes_fsm_tbl.o

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(OUTPUT_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

$(OUTPUT_DIR)/fsm_tbl_protocolo.o: fsm_tbl_protocolo.c fsm_tbl_protocolo.h | $(OUTPUT_DIR)
	$(CC) $(CFLAGS) -c fsm_tbl_protocolo.c -o $@

$(OUTPUT_DIR)/testes_fsm_tbl.o: testes_fsm_tbl.c fsm_tbl_protocolo.h | $(OUTPUT_DIR)
	$(CC) $(CFLAGS) -c testes_fsm_tbl.c -o $@

$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(OUTPUT_DIR)
