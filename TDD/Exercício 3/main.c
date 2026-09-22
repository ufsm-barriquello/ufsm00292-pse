/******************************************************************************

Implementar um módulo com funções de manipulação de um buffer circular.

*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>

/* =========================================================
   MACROS DE TESTE (Conforme material da aula)
   ========================================================= */
#define verifica(mensagem, teste) do { if (!(teste)) return mensagem; } while (0)
#define executa_teste(teste) do { char *mensagem = teste(); testes_executados++; \
if (mensagem) return mensagem; } while (0)

int testes_executados = 0;
static char * executa_testes(void);

/* =========================================================
   MÓDULO DO BUFFER CIRCULAR
   ========================================================= */
#define TAMANHO_BUFFER 5

int buffer[TAMANHO_BUFFER];
int start = 0;  // Ponteiro de leitura
int end = 0;    // Ponteiro de escrita
int count = 0;  // Quantidade de elementos atuais

// 1. Inicializa/Reseta o buffer
void inicia_buffer(void) {
    start = 0;
    end = 0;
    count = 0;
}

// 2. Verifica se está vazio
int ta_vazio(void) {
    return (count == 0);
}

// 3. Verifica se está cheio
int ta_cheio(void) {
    return (count == TAMANHO_BUFFER);
}

// 4. Insere um dado (retorna 0 se sucesso, -1 se erro)
int insere_dado(int valor) {
    if (ta_cheio()) {
        return -1; // Overflow: erro ao tentar inserir em buffer cheio
    }
    buffer[end] = valor;
    end = (end + 1) % TAMANHO_BUFFER; // Comportamento circular
    count++;
    return 0;
}

// 5. Remove um dado (retorna 0 se sucesso, -1 se erro)
int remove_dado(int *valor_lido) {
    if (ta_vazio()) {
        return -1; // Underflow: erro ao tentar ler de buffer vazio
    }
    *valor_lido = buffer[start];
    start = (start + 1) % TAMANHO_BUFFER; // Comportamento circular
    count--;
    return 0;
}

/* =========================================================
   FUNÇÕES DE TESTE (TDD)
   ========================================================= */
static char * teste_inicia_vazio(void) {
    inicia_buffer();
    verifica("erro: buffer recem criado deveria estar vazio", ta_vazio() == 1);
    return 0;
}

static char * teste_insere_nao_vazio(void) {
    inicia_buffer();
    insere_dado(10);
    verifica("erro: buffer com 1 elemento nao deveria estar vazio", ta_vazio() == 0);
    return 0;
}

static char * teste_insere_remove(void) {
    inicia_buffer();
    int valor;
    insere_dado(42);
    remove_dado(&valor);
    verifica("erro: valor lido diferente do inserido", valor == 42);
    verifica("erro: buffer deveria voltar a ficar vazio", ta_vazio() == 1);
    return 0;
}

static char * teste_underflow(void) {
    inicia_buffer();
    int valor;
    // Tentar remover de um buffer vazio deve retornar -1
    verifica("erro: remover de buffer vazio deveria retornar erro (-1)", remove_dado(&valor) == -1);
    return 0;
}

static char * teste_cheio_e_overflow(void) {
    inicia_buffer();
    
    // Enche o buffer até o limite definido (5)
    for(int i = 0; i < TAMANHO_BUFFER; i++) {
        insere_dado(i);
    }
    
    verifica("erro: buffer deveria estar cheio", ta_cheio() == 1);
    verifica("erro: inserir em buffer cheio deveria retornar erro (-1)", insere_dado(99) == -1);
    return 0;
}

static char * teste_comportamento_circular(void) {
    inicia_buffer();
    int valor;
    
    // 1. Enche o buffer
    insere_dado(1);
    insere_dado(2);
    insere_dado(3);
    insere_dado(4);
    insere_dado(5);
    
    // 2. Remove dois itens para liberar espaço no começo
    remove_dado(&valor); // Remove o 1
    remove_dado(&valor); // Remove o 2
    
    // 3. Insere mais dois itens. Eles devem dar a volta e ocupar as posições 0 e 1 do array!
    verifica("erro: deveria conseguir inserir apos liberar espaco", insere_dado(6) == 0);
    verifica("erro: deveria conseguir inserir apos liberar espaco", insere_dado(7) == 0);
    
    // 4. O próximo item a sair tem que ser o 3 (respeitando a fila FIFO)
    remove_dado(&valor);
    verifica("erro: a fila perdeu a ordem correta na volta circular", valor == 3);
    
    return 0;
}

/* =========================================================
   BLOCO DE EXECUÇÃO
   ========================================================= */
static char * executa_testes(void) {
    executa_teste(teste_inicia_vazio);
    executa_teste(teste_insere_nao_vazio);
    executa_teste(teste_insere_remove);
    executa_teste(teste_underflow);
    executa_teste(teste_cheio_e_overflow);
    executa_teste(teste_comportamento_circular);
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