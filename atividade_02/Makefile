CC = gcc
CFLAGS = -Wall -O2
TARGET = fsm_app.exe
MATLAB = matlab

# Alvo padrão: compila, roda o C e gera os gráficos
all: plot

$(TARGET): main.c
	$(CC) $(CFLAGS) main.c -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

plot: run
	$(MATLAB) -batch "gerar_graficos"

clean:
	del /f /q $(TARGET) log_fsm.csv grafico_resultado_fsm.png 2>nul || rm -f $(TARGET) log_fsm.csv grafico_resultado_fsm.png