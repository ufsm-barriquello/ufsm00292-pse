@echo off
echo ===================================================
echo            COMPILANDO E EXECUTANDO TESTES          
echo ===================================================

echo Compilando FSM do Protocolo...
gcc -Wall -Wextra -Werror protocolo.c test_protocolo.c -o test_protocolo.exe
if %errorlevel% neq 0 (
    echo [ERRO] Falha na compilacao do Protocolo!
    exit /b %errorlevel%
)

echo Executando testes...
test_protocolo.exe

echo ===================================================
echo [SUCESSO] Testes finalizados!
echo ===================================================