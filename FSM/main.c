#include <stdio.h>
#include <stdlib.h>

/* Macros de teste */
#define verifica(mensagem, teste) do { if (!(teste)) return mensagem; } while (0)
#define executa_teste(teste) do { char *mensagem = teste(); testes_executados++; \
if (mensagem) return mensagem; } while (0)

int testes_executados = 0;

// 1. Enumeração dos estados
typedef enum{
    ENVIA_STX,
    ENVIA_QTD,
    ENVIA_DADOS,
    ENVIA_CHK,
    ENVIA_ETX
} EstadoTX;

// 2. Começar pelo primeiro passo:
EstadoTX estado_tx = ENVIA_STX;

// Variáveis de controle
unsigned char qtd_a_enviar = 0;      // Tamanho da mensagem (N Bytes)
unsigned char buffer_dados[10];      // Nossa caixa de pacotes (dados)
unsigned int indice_dados = 0;       // Qual pacote estamos enviando agora
unsigned char checksum_calculado = 0;   // guarda checksum da mensagem

// capturar envio
unsigned char byte_enviado = 0;
void enviar_byte_mock(unsigned char byte) {
    byte_enviado = byte;
}

// 3. ------------------------- FSM ------------------------------
void fsm_transmissor() {
    switch(estado_tx){
        
        case ENVIA_STX:
            // TAREFA: Enviar o byte de início de transmissão (0x02)
            enviar_byte_mock(0x02);
            estado_tx = ENVIA_QTD;
            break;
            
        case ENVIA_QTD:
            // TAREFA: Enviar o tamanho da mensagem (qtd de dados)[cite: 4]
            enviar_byte_mock(qtd_a_enviar);
            // MUDA ESTADO:
            estado_tx = ENVIA_DADOS;
            break;

        case ENVIA_DADOS:
            // TAREFA: envia o pacote atual apontado pelo indice_dados
            enviar_byte_mock(buffer_dados[indice_dados]);
        
            // Atualiza o índice para o próximo pacote
            indice_dados++;
        
            // muda estado se tudo for byte_enviado
            if (indice_dados >= qtd_a_enviar) {
                estado_tx = ENVIA_CHK;
            } 
            break;
            
        case ENVIA_CHK:
            // TAREFA: Envia o byte de verificação[cite: 4]
            enviar_byte_mock(checksum_calculado);
            // MUDA ESTADO: Vai para o final
            estado_tx = ENVIA_ETX;
            break;
            
        case ENVIA_ETX:
            // TAREFA: Envia o byte de fim de transmissão (0x03)[cite: 4]
            enviar_byte_mock(0x03);
            // MUDA ESTADO: Volta para o início, pronto para uma nova mensagem
            estado_tx = ENVIA_STX;
            break;
        
        default:
            estado_tx = ENVIA_STX;
            break;
        
    }
}  
    
/* --------------------------- TESTE TDD --------------------- */

// TESTE PASSO 1
static char * teste_fsm_tx_envia_stx(void){
    // 1. Configuração 
    estado_tx = ENVIA_STX;
    byte_enviado = 0;
    
    // 2. Roda a fsm uma vez
    fsm_transmissor();
    
    // 3. Verificação
    verifica("erro: FSM não enviou o byte STX (0X02)", byte_enviado == 0x02);
    // FSM precisa ter avançado para próximo estado
    verifica("erro: FSM nao avançou para ENVIA_QTD", estado_tx == ENVIA_QTD);
    
    return 0;
}

// TESTE PASSO 2 
static char * teste_fsm_tx_envia_qtd(void) {
    estado_tx = ENVIA_QTD;
    qtd_a_enviar = 5;
    byte_enviado = 0;
    
    fsm_transmissor(); // Exercício
    
    verifica("erro: FSM nao enviou a QTD correta", byte_enviado == 5);
    verifica("erro: FSM nao avancou para ENVIA_DADOS", estado_tx == ENVIA_DADOS);
    return 0;
}

// Teste do Passo 3
static char * teste_fsm_tx_envia_dados(void) {
    estado_tx = ENVIA_DADOS;
    qtd_a_enviar = 2;               
    buffer_dados[0] = 0xAA;         
    buffer_dados[1] = 0xBB;         
    indice_dados = 0;               
    byte_enviado = 0;
    
    // Roda a 1ª vez para o dado 1
    fsm_transmissor(); 
    verifica("erro: FSM nao enviou o dado 1", byte_enviado == 0xAA);
    verifica("erro: FSM mudou de estado cedo demais", estado_tx == ENVIA_DADOS);
    
    // Roda a 2ª vez para o dado 2
    fsm_transmissor(); 
    verifica("erro: FSM nao enviou o dado 2", byte_enviado == 0xBB);
    verifica("erro: FSM nao avancou para ENVIA_CHK", estado_tx == ENVIA_CHK);
    return 0;
}

// TESTE 4 
static char * teste_fsm_tx_envia_chk(void) {
    estado_tx = ENVIA_CHK;
    checksum_calculado = 0xFF; // Valor simulado
    byte_enviado = 0;
    
    fsm_transmissor(); 
    verifica("erro: FSM nao enviou o checksum", byte_enviado == 0xFF);
    verifica("erro: FSM nao avancou para ENVIA_ETX", estado_tx == ENVIA_ETX);
    return 0;
}

// TESTE 5
static char * teste_fsm_tx_envia_etx(void) {
    estado_tx = ENVIA_ETX;
    byte_enviado = 0;
    
    fsm_transmissor(); 
    verifica("erro: FSM nao enviou o byte ETX (0x03)", byte_enviado == 0x03);
    verifica("erro: FSM nao resetou para ENVIA_STX", estado_tx == ENVIA_STX);
    return 0;
}
        
/* =========================================================
   EXECUÇÃO
   ========================================================= */
static char * executa_testes(void) {
    executa_teste(teste_fsm_tx_envia_stx);
    executa_teste(teste_fsm_tx_envia_qtd);
    executa_teste(teste_fsm_tx_envia_dados);
    executa_teste(teste_fsm_tx_envia_chk);
    executa_teste(teste_fsm_tx_envia_etx);
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
    return resultado != 0;
}