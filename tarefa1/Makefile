CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = tarefa1
SRCS = buffer.c test.c
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