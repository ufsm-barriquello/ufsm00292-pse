@echo off
cls
echo ===================================================
echo   COMPILANDO E EXECUTANDO PROJETO 4 (PROTOTHREADS)
echo ===================================================
echo.

gcc -Wall -Wextra -Werror proto_comm.c test_proto_comm.c -o test_p4.exe
if %errorlevel% neq 0 (
    echo [ERRO] Falha na compilacao do Projeto 4!
    pause
    exit /b %errorlevel%
)

test_p4.exe
echo.
pause