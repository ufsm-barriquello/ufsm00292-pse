# Projeto 4 - Comunicação Confiável com Protothreads e TDD

Este repositório contém a implementação do codificador e decodificador do protocolo de comunicação serial `| STX | QTD | DADOS | CHK | ETX |` utilizando **Protothreads** (concorrência cooperativa *stackless*) e **TDD**.

---

## 🛠️ Arquitetura das Protothreads

* **Protothread Transmissora (`pt_transmissor_thread`):** Formata os dados no pacote, envia os bytes via canal e aguarda a confirmação de recebimento (`ACK`). Em caso de falha ou estouro de *Timeout*, realiza a retransmissão automática.
* **Protothread Receptora (`pt_receptor_thread`):** Monitora o barramento, decodifica o pacote byte a byte, valida o Checksum e responde com `ACK` (sucesso) ou `NACK` (erro).

---

## 📁 Estrutura do Projeto

* `pt.h`: Biblioteca interna de Protothreads de Adam Dunkels.
* `proto_comm.h`: Interface, estruturas do canal simulado e protótipos das threads TX/RX.
* `proto_comm.c`: Lógica das protothreads transmissora e receptora.
* `test_proto_comm.c`: Suíte de testes unitários em TDD (valida envio, ACK/NACK e retransmissão).
* `run_tests.sh`: Script de automação para compilação e execução no **Linux**.
* `run_tests.bat`: Script de automação para compilação e execução no **Windows (CMD)**.

---

## 🚀 Como Executar os Testes

### 🐧 No Linux (Terminal)
1. Conceda permissão de execução:
   ```bash
   chmod +x run_tests.sh