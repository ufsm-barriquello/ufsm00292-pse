#!/bin/bash

# Limpa a tela
clear

echo "==================================================="
echo "  COMPILANDO E EXECUTANDO PROJETO 3 (TDD/FSM TABELA)"
echo "==================================================="
echo ""

# Compila os arquivos fonte C com flags de warning e erro ativadas
gcc -Wall -Wextra -Werror fsm_protocolo.c test_fsm_protocolo.c -o test_p3

# Verifica se a compilação foi bem-sucedida ($? retorna o código de saída do GCC)
if [ $? -ne 0 ]; then
    echo "[ERRO] Falha na compilação do Projeto 3!"
    exit 1
fi

# Executa o binário
./test_p3