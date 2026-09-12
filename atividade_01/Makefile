CC = gcc
CFLAGS = -Wall -Wextra -g

TARGET = testes.exe
LOGFILE = resultados_testes.txt

SRCS = atividade_one.c buffer.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	@if exist $(LOGFILE) del /f /q $(LOGFILE)
	.\$(TARGET) > $(LOGFILE) 2>&1
	@type $(LOGFILE)

clean:
	del /f /q *.o $(TARGET) $(LOGFILE)