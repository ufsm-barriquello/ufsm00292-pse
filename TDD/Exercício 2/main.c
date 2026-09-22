/******************************************************************************

Escrever uma função que converte um número decimal de 0 a 9999 em um número
no formato BCD (binary-coded-decimal)

*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>

/* =========================================================
   MACROS DE TESTE 
   ========================================================= */
#define verifica(mensagem, teste) do { if (!(teste)) return mensagem; } while (0)
#define executa_teste(teste) do { char *mensagem = teste(); testes_executados++; \
if (mensagem) return mensagem; } while (0)

int testes_executados = 0;
static char * executa_testes(void);

/* =========================================================
   FUNÇÃO PRINCIPAL (Decimal para BCD)
   ========================================================= */
unsigned int dec2bcd(unsigned int dec) {
    unsigned int resultado_bcd = 0;
    int deslocamento = 0; 
    
    while (dec > 0) {
        // Extrai o dígito menos significativo (ex: 123 -> pega o 3)
        unsigned int digito = dec % 10; 
        
        // Posiciona os 4 bits do dígito no local correto do BCD
        resultado_bcd = resultado_bcd | (digito << deslocamento);
        
        // Remove o último dígito do número original (ex: 123 -> vira 12)
        dec = dec / 10;
        
        // Incrementa o deslocamento em 4 bits (um nibble) para o próximo dígito
        deslocamento = deslocamento + 4; 
    }
    
    return resultado_bcd;
}

/* =========================================================
   FUNÇÕES DE TESTE (TDD)
   ========================================================= */
static char * teste_converte_0(void) {
    verifica("erro: dec2bcd(0) deveria retornar 0x0", dec2bcd(0) == 0x0);
    return 0;
}

static char * teste_converte_10(void) {
    verifica("erro: dec2bcd(10) deveria retornar 0x10", dec2bcd(10) == 0x10);
    return 0;
}

static char * teste_converte_99(void) {
    verifica("erro: dec2bcd(99) deveria retornar 0x99", dec2bcd(99) == 0x99);
    return 0;
}

static char * teste_converte_9999(void) {
    // 9999 é o limite máximo pedido no enunciado
    verifica("erro: dec2bcd(9999) deveria retornar 0x9999", dec2bcd(9999) == 0x9999);
    return 0;
}

/* =========================================================
   BLOCO DE EXECUÇÃO
   ========================================================= */
static char * executa_testes(void) {
    executa_teste(teste_converte_0);
    executa_teste(teste_converte_10);
    executa_teste(teste_converte_99);
    executa_teste(teste_converte_9999);
    return 0;
}

int main() {
    char *resultado = executa_testes();
    
    if (resultado != 0) {
        // Se algum teste falhou
        printf("%s\n", resultado);
    } else {
        // Sucesso total
        printf("TODOS OS TESTES PASSARAM\n");
    }
    
    printf("Testes executados: %d\n", testes_executados);
    return resultado != 0;
}