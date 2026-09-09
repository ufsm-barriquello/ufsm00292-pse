CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = tarefa
SRCS = protocolo.c test.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean test

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	./$(TARGET)

all: test

clean:
	rm -f $(OBJS) $(TARGET)