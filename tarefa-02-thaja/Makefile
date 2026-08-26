CC = gcc
CFLAGS = -Wall -Wextra -Wconversion -std=c99 -I.
OUTDIR = output
SRCS = fsm_protocolo.c testes_fsm.c
OBJS = $(addprefix $(OUTDIR)/, $(SRCS:.c=.o))
TARGET = $(OUTDIR)/testes_fsm

all: $(TARGET)

$(OUTDIR):
	mkdir -p $(OUTDIR)

$(TARGET): $(OBJS) | $(OUTDIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

$(OUTDIR)/%.o: %.c fsm_protocolo.h | $(OUTDIR)
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -rf $(OUTDIR)

.PHONY: all run clean