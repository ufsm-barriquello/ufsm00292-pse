CC = gcc
CFLAGS = -Wall -Wextra -Wno-implicit-fallthrough -std=c99 -O2
TARGET = protothread_app.exe

all: test

$(TARGET): main.c pt.h
	$(CC) $(CFLAGS) main.c -o $(TARGET)

test: $(TARGET)
	./$(TARGET)

clean:
	del /f /q $(TARGET) *.o 2>nul || rm -f $(TARGET) *.o