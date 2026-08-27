@echo off
cls

:: Adiciona o caminho do GCC temporariamente nesta sessao do terminal
:: (Ajuste a pasta abaixo conforme a instalacao do seu computador)
set PATH=C:\MinGW\bin;%PATH%

echo ===================================================
echo   COMPILANDO E EXECUTANDO PROJETO 3 (TDD/FSM TABELA)
echo ===================================================
echo.

gcc -Wall -Wextra -Werror fsm_protocolo.c test_fsm_protocolo.c -o test_p3.exe
if %errorlevel% neq 0 (
    echo [ERRO] Falha na compilacao do Projeto 3!
    pause
    exit /b %errorlevel%
)

test_p3.exe
echo.
pause