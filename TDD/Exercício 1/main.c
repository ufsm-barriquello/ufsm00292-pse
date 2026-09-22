/******************************************************************************

Escrever uma função para encontrar e retornar a posição do primeiro
bit igual a 1, em uma bariável de 32 bits sem sinal,
a partir do bit mmais significativo.

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
   FUNÇÃO PRINCIPAL (Refatorada)
   ========================================================= */
int encontra_primeiro_bit(unsigned int x) {
    // Se o número for 0, retorna -1 indicando que não há bits em nível 1
    if (x == 0) return -1;
    
    // Varre os 32 bits a partir do mais significativo (posição 31 até 0)
    for (int i = 31; i >= 0; i--) {
        // Desloca o bit alvo para a posição 0 e aplica uma máscara AND
        if ((x >> i) & 1) {
            return i; // Retorna a posição do primeiro bit 1 encontrado
        }
    }
    
    return -1;
}

/* =========================================================
   FUNÇÕES DE TESTE (TDD)
   ========================================================= */
static char * teste_numero_zero_retorna_erro(void) {
    verifica("erro: encontra_primeiro_bit(0) deveria retornar -1", encontra_primeiro_bit(0) == -1);
    return 0;
}

static char * teste_numero_1_retorna_0(void) {
    // Binário: ...00000001 (bit 1 na posição 0)
    verifica("erro: encontra_primeiro_bit(1) deveria retornar 0", encontra_primeiro_bit(1) == 0);
    return 0;
}

static char * teste_exemplo_dos_slides(void) {
    // O slide dá o exemplo: 001100111 => posição 6
    // 001100111 em binário é igual a 103 em decimal.
    verifica("erro: encontra_primeiro_bit(103) deveria retornar 6", encontra_primeiro_bit(103) == 6);
    return 0;
}

static char * teste_bit_mais_significativo(void) {
    // 0x80000000 em hexadecimal equivale a um número com apenas o bit 31 igual a 1.
    verifica("erro: encontra_primeiro_bit(0x80000000) deveria retornar 31", encontra_primeiro_bit(0x80000000) == 31);
    return 0;
}

/* =========================================================
   BLOCO DE EXECUÇÃO
   ========================================================= */
static char * executa_testes(void) {
    executa_teste(teste_numero_zero_retorna_erro);
    executa_teste(teste_numero_1_retorna_0);
    executa_teste(teste_exemplo_dos_slides);
    executa_teste(teste_bit_mais_significativo);
    return 0;
}

int main() {
    char *resultado = executa_testes();
    
    if (resultado != 0) {
        // Se algum teste falhou, imprime a mensagem de erro
        printf("%s\n", resultado);
    } else {
        // Se passou por tudo ileso
        printf("TODOS OS TESTES PASSARAM\n");
    }
    
    printf("Testes executados: %d\n", testes_executados);
    return resultado != 0;
}