# Descrição do protocolo implementado
|STX|QTD_DADOS|DADOS|CHK|ETX|
STX -> 1 Byte (0x02)
QTD_DADOS -> 1 Byte
DADOS -> QTD_DADOS bytes
CHK -> 1 Byte
ETX -> 1 Byte (0x03)

## Dados da implementação
QTD_DADOS é um valor unsigned de 8 bits (1 Byte)
O checksum é computado via XOR de todos os bytes de dados
Se o checksum não bate, o estado volta para start.