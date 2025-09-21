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

No projeto do sistema embarcado deve haver duas
protothreads. A transmissora, que envia dados
usando o protocolo especificado; e a receptora, que
recebe os dados e interpreta o protocolo.
Se os dados estiverem corretos e no formato correto, a
receptora envia uma confirmação (ACK) para a
transmissora. Em caso de erro, a transmissora reenvia
os dados após um tempo máximo de espera.

Faça a implementação em linguagem C, usando protothread.