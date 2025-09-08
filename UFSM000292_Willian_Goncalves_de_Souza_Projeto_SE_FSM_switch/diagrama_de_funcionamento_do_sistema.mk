```mermaid
stateDiagram-v2
    [*] --> IDLE
    IDLE --> AGUARDA_QTD: Recebe STX (0x02)
    
    AGUARDA_QTD --> PROCESS_DADOS: Recebe QTD_DADOS
    PROCESS_DADOS --> AGUARDA_CHK: Todos DADOS recebidos
    
    AGUARDA_CHK --> COMPLETA: Recebe CHK
    COMPLETA --> IDLE: Recebe ETX (processa pacote)
    ```
