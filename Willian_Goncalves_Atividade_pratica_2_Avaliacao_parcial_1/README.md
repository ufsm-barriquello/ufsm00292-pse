# Avaliação Parcial 1 (Parte 2) - FSM do Protocolo de Comunicação

Este repositório contém a implementação do tratamento de um protocolo de comunicação serial utilizando **Máquinas de Estados Finitos (FSM)** em linguagem C e desenvolvida sob a metodologia **Test-Driven Development (TDD)**.

## 📋 Formato do Protocolo

| STX (1 Byte) | QTD_DADOS (1 Byte) | DADOS (N Bytes) | CHK (1 Byte) | ETX (1 Byte) |
| :---: | :---: | :---: | :---: | :---: |
| `0x02` | `N` | Payload | Soma 8-bits | `0x03` |

---

## 🛠️ Estrutura do Projeto

* `protocolo.h`: Interface pública, estruturas e enums dos estados das FSMs (TX e RX).
* `protocolo.c`: Implementação do transmissor e receptor usando a diretiva `switch-case`.
* `test_protocolo.c`: Suíte de testes unitários baseada no framework leve *MinUnit*.
* `run_tests.bat`: Script de automação para compilação e execução rápida no Windows.

---

## 🚀 Como Executar os Testes

No terminal (CMD) dentro do diretório do projeto, execute o script batch:

```cmd
run_tests.bat