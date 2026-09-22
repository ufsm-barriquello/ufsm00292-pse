#include <stdio.h>
#include <stdlib.h>

/* enunciado */

#define STX 0x02
#define ETX 0x03
#define MAX_BUFFER 256

/* ----------------------------
    Macro de Teste (TDD)
    ------------------------ */
    
#define verifica(mensagem, teste) do { if (!(teste)) return mensagem; } while (0)
#define executa_teste(teste) do { char *mensagem = teste(); testes_executados++; \
if (mensagem) return mensagem; } while (0)

int testes_executados = 0;

/* ---------------------------------
   1. Esteuturas do Receptor (FSM com Funtion Pointer)
   --------------------------------- */
   
// Possíveis estados da máquina de estados de comunicação
typedef enum {
    ST_STX = 0,
    ST_QTD, 
    ST_DATA, 
    ST_CHK, 
    ST_ETX
} States;

// Definição do ponteiro de função, indicando que todas as funções de estado receberão um "unsigned char" 
typedef void (*Action)(unsigned char data);

// Estrutura principal da FSM
struct StateMachine {
    States state;
    unsigned char buffer[MAX_BUFFER];
    unsigned char chkBuffer;
    int indBuffer;
    int qtdBuffer;
    Action action[5]; // tabela de eventos
} sm;

/* -------------------------------------
    2. Funções de Cada estado
    ----------------------------------- */
void stSTX(unsigned char data) {
    if (data == STX) {
        sm.indBuffer = sm.qtdBuffer = 0;    // oreoaraprepara para receber
        sm.chkBuffer = 0;                   // zera o checksum
        sm.state = ST_QTD;                  // inicio dos dados, avança o estado
    }
}

void stQtd(unsigned char data) {
    sm.qtdBuffer = data;        // salva a quantidade de dados
    sm.state = ST_DATA;         // avança para leitura de dados
}

void stData(unsigned char data) {
    sm.buffer[sm.indBuffer++] = data; // salva o byte no buffer
    
    // calcula soma de verificação (usando XOR pra acumular)
    sm.chkBuffer ^= data;
    
    if (--sm.qtdBuffer == 0) { // se todos os bytes chegarem 
        sm.state = ST_CHK;      // aavança pra verificar checksum
    }
}

void stChk(unsigned char data) {
    if (data == sm.chkBuffer) { // testa checksum recebido contra o calculado
        sm.state = ST_ETX;      // SE correto, vai pro fim
    } else {
        sm.state = ST_STX;      // se erro, aborta e volta pro inicio
    }
}

void stETX(unsigned char data) {
    if (data == ETX) {
        // sucesso total
        // dados seriam proessados: handlePackage(sm.buffer, sm.indBuffer);
    }
    sm.state = ST_STX; // reseta para escutar próximo pacote
}

/* -----------------------------------
    3. Inicialização e Execução
    -------------------------------- */
void initSM() {
    sm.state = ST_STX;                   // FSM começa no STX
    sm.buffer[0] = 0;                    
    sm.chkBuffer = 0;                    
    sm.indBuffer = 0;                    
    sm.qtdBuffer = 0;                    

    // Mapeando a tabela de estados com os ponteiros das funções
    sm.action[ST_STX] = stSTX;           
    sm.action[ST_QTD] = stQtd;           
    sm.action[ST_DATA] = stData;         
    sm.action[ST_CHK] = stChk;           
    sm.action[ST_ETX] = stETX;           
}
    
// inteja 1 bbyte na máquina
void fsm_receptor_recebe_byte(unsigned char data) {
    // execução da FSM chamando a função correta atraves da tabela
    sm.action[sm.state](data);
}

/* -----------------------------------------------
    4. Testes TDD (Simualando recepção)
    -------------------------------------------- */
static char * teste_recebe_stx(void) {
    initSM(); 
    fsm_receptor_recebe_byte(STX); 
    verifica("erro: Nao avancou para ST_QTD apos receber STX", sm.state == ST_QTD);
    return 0;
}

static char * teste_recebe_qtd(void) {
    initSM();
    fsm_receptor_recebe_byte(STX);
    fsm_receptor_recebe_byte(2); // Simula que vai receber 2 bytes
    verifica("erro: Nao avancou para ST_DATA", sm.state == ST_DATA);
    verifica("erro: Quantidade gravada esta errada", sm.qtdBuffer == 2);
    return 0;
}

static char * teste_recebe_dados_e_valida_checksum(void) {
    initSM();
    fsm_receptor_recebe_byte(STX);
    fsm_receptor_recebe_byte(2); // QTD = 2
    
    // Injeta Dado 1
    fsm_receptor_recebe_byte(0xAA);
    verifica("erro: Nao armazenou dado 1 no buffer", sm.buffer[0] == 0xAA);
    verifica("erro: Mudou de estado antes da hora", sm.state == ST_DATA);
    
    // Injeta Dado 2
    fsm_receptor_recebe_byte(0x11);
    verifica("erro: Nao armazenou dado 2 no buffer", sm.buffer[1] == 0x11);
    verifica("erro: Nao avancou para o CHK apos o ultimo dado", sm.state == ST_CHK);
    
    // Injeta Checksum correto (0xAA XOR 0x11 = 0xBB)
    fsm_receptor_recebe_byte(0xBB);
    verifica("erro: Falha no Checksum, nao foi para ST_ETX", sm.state == ST_ETX);
    
    // Injeta Final de Transmissão
    fsm_receptor_recebe_byte(ETX);
    verifica("erro: Nao reiniciou a maquina ao final do protocolo", sm.state == ST_STX);
    
    return 0;
}

/* =========================================================
   BLOCO DE EXECUÇÃO
   ========================================================= */
static char * executa_testes(void) {
    executa_teste(teste_recebe_stx);
    executa_teste(teste_recebe_qtd);
    executa_teste(teste_recebe_dados_e_valida_checksum);
    return 0;
}

int main() {
    char *resultado = executa_testes();
    
    if (resultado != 0) {
        printf("%s\n", resultado);
    } else {
        printf("TODOS OS TESTES PASSARAM\n");
    }
    
    printf("Testes executados: %d\n", testes_executados);
    return 0;
}

