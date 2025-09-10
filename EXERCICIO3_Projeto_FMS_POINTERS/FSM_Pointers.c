#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_BUFFER 255
#define STX 0x02
#define ETX 0x03

typedef enum {
    ST_STX = 0, ST_QTD, ST_DATA, ST_CHK, ST_ETX
} States;

typedef void (*Action)(unsigned char data); // Ponteiro de função

// Estrutura da FSM
struct StateMachine {
    States state;
    unsigned char buffer[MAX_BUFFER];
    unsigned char chkBuffer;
    int indBuffer;
    int qtdBuffer;
    Action action[5];
} sm;

// --- Protótipos das Funções ---
void stSTX(unsigned char data);
void stQtd(unsigned char data);
void stData(unsigned char data);
void stChk(unsigned char data);
void stETX(unsigned char data);
void initSM();
void handleRx(unsigned char* data, int qtd);
void handlePackage(const unsigned char* pkg_buffer, int pkg_len);

// Estrutura para capturar o resultado para verificação nos testes
struct TestResult {
    bool package_handled;
    unsigned char buffer[MAX_BUFFER];
    int len;
};
struct TestResult test_result;

// Macros de Teste
#define verifica(mensagem, teste) do { if (!(teste)) return mensagem; } while (0)
#define executa_teste(teste) do { char *mensagem = teste(); testes_executados++; \
                                if (mensagem) return mensagem; } while (0)
int testes_executados = 0;

void setup_teste() {
    test_result.package_handled = false;
    test_result.len = 0;
    memset(test_result.buffer, 0, MAX_BUFFER);
    initSM();
}

static char* teste_pacote_valido() {
    setup_teste();
    unsigned char pacote[] = {STX, 0x04, 0x0A, 0x0B, 0x0C, 0x0D, (0x0A^0x0B^0x0C^0x0D), ETX};
    
    handleRx(pacote, sizeof(pacote));
    
    verifica("TDD ERRO: Pacote valido deveria ter sido tratado", test_result.package_handled == true);
    verifica("TDD ERRO: Tamanho do pacote incorreto", test_result.len == 4);
    verifica("TDD ERRO: Conteudo do pacote incorreto (byte 0)", test_result.buffer[0] == 0x0A);
    verifica("TDD ERRO: Conteudo do pacote incorreto (byte 3)", test_result.buffer[3] == 0x0D);
    verifica("TDD ERRO: Estado final da FSM deveria ser ST_STX", sm.state == ST_STX);
    return 0;
}

static char* teste_pacote_checksum_invalido() {
    setup_teste();
    unsigned char pacote[] = {STX, 0x02, 0x10, 0x20, 0x99 /*chk errado*/, ETX};
    
    handleRx(pacote, sizeof(pacote));
    
    verifica("TDD ERRO: Pacote com checksum invalido nao deveria ser tratado", test_result.package_handled == false);
    verifica("TDD ERRO: FSM deveria retornar para ST_STX apos erro de checksum", sm.state == ST_STX);
    return 0;
}

static char* teste_ignora_lixo_antes_do_pacote() {
    setup_teste();
    unsigned char pacote[] = {0xFF, 0xEE, 0xDD, STX, 0x02, 'A', 'B', ('A'^'B'), ETX};
    
    handleRx(pacote, sizeof(pacote));
    
    verifica("TDD ERRO: Pacote valido apos lixo deveria ser tratado", test_result.package_handled == true);
    verifica("TDD ERRO: Tamanho do pacote (apos lixo) incorreto", test_result.len == 2);
    verifica("TDD ERRO: Conteudo do pacote (apos lixo) incorreto", test_result.buffer[1] == 'B');
    return 0;
}

static char* executa_todos_testes() {
    executa_teste(teste_pacote_valido);
    executa_teste(teste_pacote_checksum_invalido);
    executa_teste(teste_ignora_lixo_antes_do_pacote);
    return 0;
}

int main() {
    char *resultado = executa_todos_testes();
    
    printf("\n--- Resultado Final com TDD ---\n");
    if (resultado != 0) {
        printf("FALHOU: %s\n", resultado);
    } else {
        printf("TODOS OS TESTES PASSARAM\n");
    }
    printf("Testes executados: %d\n", testes_executados);
    
    return resultado != 0;
}


// --- Implementação da Lógica da FSM ---

void handlePackage(const unsigned char* pkg_buffer, int pkg_len) {
    test_result.package_handled = true;
    test_result.len = pkg_len;
    memcpy(test_result.buffer, pkg_buffer, pkg_len);
}

void stSTX(unsigned char data) {
    if (data == STX) {
        sm.indBuffer = sm.qtdBuffer = 0;
        sm.chkBuffer = 0;
        sm.state = ST_QTD;
    }
}

void stQtd(unsigned char data) {
    sm.qtdBuffer = data;
    if (sm.qtdBuffer > 0) {
        sm.state = ST_DATA;
    } else {
        sm.state = ST_CHK;
    }
}

void stData(unsigned char data) {
    sm.buffer[sm.indBuffer++] = data;
    sm.chkBuffer ^= data;

    if (--sm.qtdBuffer == 0) {
        sm.state = ST_CHK;
    }
}

void stChk(unsigned char data) {
    if (data == sm.chkBuffer) {
        sm.state = ST_ETX;
    } else {
        sm.state = ST_STX;
    }
}

void stETX(unsigned char data) {
    if (data == ETX) {
        handlePackage(sm.buffer, sm.indBuffer);
    }
    sm.state = ST_STX;
}

void handleRx(unsigned char* data, int qtd) {
    for (int i = 0; i < qtd; i++) {
        sm.action[sm.state](data[i]);
    }
}

void initSM() {
    sm.state = ST_STX;
    sm.indBuffer = 0;
    sm.qtdBuffer = 0;
    sm.chkBuffer = 0;
    
    sm.action[ST_STX] = stSTX;
    sm.action[ST_QTD] = stQtd;
    sm.action[ST_DATA] = stData;
    sm.action[ST_CHK] = stChk;
    sm.action[ST_ETX] = stETX;
}
