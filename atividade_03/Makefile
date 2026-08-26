CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
TARGET = test_fsm.exe

all: test

$(TARGET): fsm.c test_fsm.c fsm.h
	$(CC) $(CFLAGS) fsm.c test_fsm.c -o $(TARGET)

test: $(TARGET)
	./$(TARGET)

clean:
	del /f /q $(TARGET) *.o 2>nul || rm -f $(TARGET) *.o