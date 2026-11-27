#include <stdio.h>
#include <stdbool.h>

// ---------- Macros ----------
#define STX 0x02  // Inicio da transmissao
#define ETX 0x03  // Fim da transmissao

#define verifica(msg, cond) if (!(cond)) return msg;

// ---------- Definicoes ----------
typedef enum {
    TRANSMISSOR_OCIOSO,
    TRANSMISSOR_STX,
    TRANSMISSOR_QTD,
    TRANSMISSOR_DADOS,
    TRANSMISSOR_CHK,
    TRANSMISSOR_ETX
} EstadoTransmissor;

typedef enum {
    RECEPTOR_OCIOSO,
    RECEPTOR_STX,
    RECEPTOR_QTD,
    RECEPTOR_DADOS,
    RECEPTOR_CHK,
    RECEPTOR_ETX
} EstadoReceptor;

// ---------- Variaveis globais para teste ----------
EstadoReceptor estadoFinalReceptor = RECEPTOR_OCIOSO;
unsigned char qtdRecebida = 0;
unsigned char chkRecebido = 0;
unsigned char chkCalculado = 0;

// ---------- Funcoes auxiliares ----------
unsigned char calcularChecksum(char* dados, unsigned char qtd) {
    unsigned char checksum = 0;
    for (int i = 0; i < qtd; i++) {
        checksum += dados[i];
    }
    return checksum;
}

// ---------- FSM Transmissor ----------
void fsmTransmissor(char* dados, unsigned char qtdDados) {
    EstadoTransmissor estado = TRANSMISSOR_OCIOSO;
    unsigned char checksum = calcularChecksum(dados, qtdDados);
    bool transmissaoCompleta = false;
    int i = 0;

    while (!transmissaoCompleta) {
        switch (estado) {
            case TRANSMISSOR_OCIOSO:
                printf("Transmissor: Estado OCIOSO.\n");
                estado = TRANSMISSOR_STX;
                break;

            case TRANSMISSOR_STX:
                printf("Transmissor: Enviando STX.\n");
                printf("STX: 0x%02X\n", STX);
                estado = TRANSMISSOR_QTD;
                break;

            case TRANSMISSOR_QTD:
                printf("Transmissor: Enviando quantidade de dados.\n");
                printf("QTD_DADOS: %d\n", qtdDados);
                estado = TRANSMISSOR_DADOS;
                break;

            case TRANSMISSOR_DADOS:
                printf("Transmissor: Enviando dados.\n");
                for (i = 0; i < qtdDados; i++) {
                    printf("Dado[%d]: %c\n", i, dados[i]);
                }
                estado = TRANSMISSOR_CHK;
                break;

            case TRANSMISSOR_CHK:
                printf("Transmissor: Enviando checksum.\n");
                printf("CHK: 0x%02X\n", checksum);
                estado = TRANSMISSOR_ETX;
                break;

            case TRANSMISSOR_ETX:
                printf("Transmissor: Enviando ETX.\n");
                printf("ETX: 0x%02X\n", ETX);
                transmissaoCompleta = true;
                estado = TRANSMISSOR_OCIOSO;
                break;

            default:
                printf("Transmissor: Estado desconhecido.\n");
                estado = TRANSMISSOR_OCIOSO;
                break;
        }
    }
}

// ---------- FSM Receptor ----------
void fsmReceptor(char* dadosRecebidos, unsigned char qtdDados, unsigned char chkRecebidoExternamente) {
    EstadoReceptor estado = RECEPTOR_OCIOSO;
    estadoFinalReceptor = estado;
    chkCalculado = calcularChecksum(dadosRecebidos, qtdDados);
    chkRecebido = chkRecebidoExternamente;
    qtdRecebida = qtdDados;
    bool recepcaoCompleta = false;
    int i = 0;

    while (!recepcaoCompleta) {
        switch (estado) {
            case RECEPTOR_OCIOSO:
                printf("Receptor: Estado OCIOSO.\n");
                estado = RECEPTOR_STX;
                break;

            case RECEPTOR_STX:
                printf("Receptor: Recebendo STX.\n");
                printf("STX recebido: 0x%02X\n", STX);
                estado = RECEPTOR_QTD;
                break;

            case RECEPTOR_QTD:
                printf("Receptor: Recebendo quantidade de dados.\n");
                printf("QTD_DADOS recebido: %d\n", qtdDados);
                estado = RECEPTOR_DADOS;
                break;

            case RECEPTOR_DADOS:
                printf("Receptor: Recebendo dados.\n");
                for (i = 0; i < qtdDados; i++) {
                    printf("Dado[%d]: %c\n", i, dadosRecebidos[i]);
                }
                estado = RECEPTOR_CHK;
                break;

            case RECEPTOR_CHK:
                printf("Receptor: Recebendo checksum.\n");
                printf("CHK recebido: 0x%02X\n", chkRecebidoExternamente);
                if (chkCalculado == chkRecebidoExternamente) {
                    printf("Checksum correto!\n");
                } else {
                    printf("Erro de checksum! Calculado: 0x%02X, Recebido: 0x%02X\n", chkCalculado, chkRecebidoExternamente);
                }
                estado = RECEPTOR_ETX;
                break;

            case RECEPTOR_ETX:
                printf("Receptor: Recebendo ETX.\n");
                printf("ETX recebido: 0x%02X\n", ETX);
                recepcaoCompleta = true;
                estado = RECEPTOR_OCIOSO;
                break;

            default:
                printf("Receptor: Estado desconhecido.\n");
                estado = RECEPTOR_OCIOSO;
                break;
        }
        estadoFinalReceptor = estado;
    }
}

// ---------- Testes ----------
char* teste_calcularChecksum() {
    char dados[] = {'T', 'E', 'S', 'T', 'E'};
    int esperado = 'T' + 'E' + 'S' + 'T' + 'E';
    int resultado = calcularChecksum(dados, sizeof(dados));
    verifica("Teste de calculo de checksum falhou!", resultado == esperado);
    return NULL;
}

char* teste_transmissaoRecebimento() {
    char dados[] = {'T', 'E', 'S', 'T', 'E', ' ', 'F', 'S', 'M'};
    unsigned char qtdDados = sizeof(dados);
    unsigned char checksum = calcularChecksum(dados, qtdDados);

    printf("Iniciando FSM do Transmissor:\n");
    fsmTransmissor(dados, qtdDados);

    printf("\nIniciando FSM do Receptor:\n");
    fsmReceptor(dados, qtdDados, checksum);

    verifica("Estado final do receptor incorreto!", estadoFinalReceptor == RECEPTOR_OCIOSO);
    verifica("Quantidade recebida incorreta!", qtdRecebida == qtdDados);
    verifica("Checksum incorreto!", chkRecebido == chkCalculado);

    return NULL;
}

// ---------- Ponteiro de função para testes ----------
typedef char* (*TesteFunc)();

void executa_teste(TesteFunc teste, const char* nome) {
    char* mensagem = teste();
    if (mensagem) printf("FALHOU: %s\n", mensagem);
    else printf("PASSOU: %s\n", nome);
}

// ---------- Main ----------
int main() {
    printf("Executando testes...\n");

    executa_teste(teste_calcularChecksum, "teste_calcularChecksum");
    executa_teste(teste_transmissaoRecebimento, "teste_transmissaoRecebimento");

    printf("Todos os testes foram executados.\n");
    return 0;
}
