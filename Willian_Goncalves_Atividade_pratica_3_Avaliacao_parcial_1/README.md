# Projeto 3 - Decodificador FSM com Tabela de Estados e Ponteiros de Função (TDD)

Este repositório contém a implementação do decodificador para o protocolo de comunicação serial `| STX (1B) | QTD (1B) | DADOS (NB) | CHK (1B) | ETX (1B) |`. A solução foi desenvolvida em C utilizando **Desenvolvimento Guiado por Testes (TDD)** e uma **Máquina de Estados Finitos (FSM) orientada por Tabela Matricial e Ponteiros de Função**.

---

## 🛠️ Arquitetura do Sistema

A FSM foi projetada utilizando uma **Matriz Bidimensional de Transição de Estados/Eventos** com complexidade de acesso $O(1)$:

* **Estados (`State`):** `STATE_STX`, `STATE_QTD`, `STATE_DADOS`, `STATE_CHK`, `STATE_ETX`.
* **Eventos (`Event`):** `EVENT_BYTE_RX` (recepção de byte) e tratamentos de erro.
* **Tabela de Transição (`StateTable`):** Matriz que associa o par `[Estado][Evento]` à sua respectiva ação (executada via **Ponteiro de Função**) e ao próximo estado do sistema.

---

## 📁 Estrutura do Projeto

* `fsm_protocolo.h`: Declaração dos enums de estados/eventos, estruturas de dados, tipo do ponteiro de função e protótipos.
* `fsm_protocolo.c`: Implementação da tabela de estados, manipuladores de ação via ponteiros de função e lógica do transmissor (TX) e receptor (RX).
* `test_fsm_protocolo.c`: Suíte de testes unitários desenvolvida em TDD cobrindo casos de sucesso e tratamento de falhas.
* `run_tests.sh`: Script de automação para compilação e execução no **Linux**.
* `run_tests.bat`: Script de automação para compilação e execução no **Windows (CMD)**.

---

## 🚀 Como Executar os Testes

### 🐧 No Linux (Terminal)
1. Dê permissão de execução ao script:
   ```bash
   chmod +x run_tests.sh