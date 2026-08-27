#!/bin/bash

# Limpa a tela
clear

echo "==================================================="
echo "  COMPILANDO E EXECUTANDO PROJETO 4 (PROTOTHREADS) "
echo "==================================================="
echo ""

# Compila os fontes C com tratamento rigoroso de avisos/erros
gcc -Wall -Wextra -Werror proto_comm.c test_proto_comm.c -o test_p4

# Verifica se a compilação teve sucesso
if [ $? -ne 0 ]; then
    echo "[ERRO] Falha na compilação do Projeto 4!"
    exit 1
fi

# Executa o binário de testes
./test_p4