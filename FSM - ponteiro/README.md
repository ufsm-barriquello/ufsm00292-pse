# Exercício

Um problema que pode ser resolvido com máquina de
estados é o tratamento de **protocolos de comunicação**.
Considere que o protocolo de comunicação tem o seguinte
formato:

| Campo      | Tamanho    | Descrição                        |                          |
|------------|------------|----------------------------------|--------------------------|
| **STX**    | 1 Byte     | Início da transmissão            | 0x02                     |
| **QTD_DADOS** | 1 Byte  | Quantidade de dados              |                          |
| **DADOS**  | N Bytes    | Dados transmitidos               |                          |
| **CHK**    | 1 Byte     | Checksum da transmissão          |                          |
| **ETX**    | 1 Byte     | Fim da transmissão               | 0x03                     |

1) Desenhe um diagrama de funcionamento do sistema.
2) Implemente o tratamento do protocolo usando máquina
de estados, tanto do transmissor quanto do receptor.

Faça a implementação em linguagem C, usando a diretiva
ponteiro.
