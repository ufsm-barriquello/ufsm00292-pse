/*
 * rtos.c
 *
 */ 

#include "rtos.h"

/* variaveis do sistema multitarefas */
uint8_t      tarefa_atual, proxima_tarefa;
tcb_t        TCB[NUMERO_DE_TAREFAS+1];
stackptr_t   ponteiro_de_pilha;
stackptr_t   SP;  /* corrigido de uint32_t para stackptr_t */
prioridade_t Prioridades[PRIORIDADE_MAXIMA+1];   /* vetor com as prioridades das tarefas */

/* variavel auxiliar para guardar o numero de marcas de tempo */
static tick_t contador_marcas = 0;

static uint8_t numero_tarefas = 0;

/* funcao para realizar o escalonamento de tarefas por prioridades */
uint8_t escalonador(void)
{
    uint8_t prioridade;
    uint8_t tarefa_selecionada = 0;
    
    /* percorre da maior prioridade até a menor */
    for (prioridade = PRIORIDADE_MAXIMA; prioridade > 0; prioridade--) { 
        if (Prioridades[prioridade] != 0) {        
            tarefa_selecionada = Prioridades[prioridade];
            if (TCB[tarefa_selecionada].estado == PRONTA) {    
                /* retorna a tarefa pronta com maior prioridade */		
                return tarefa_selecionada;    
            }
        }
    } 
    
    /* caso nenhuma esteja pronta, retorna a de menor prioridade */
    if (prioridade == 0) {
        tarefa_selecionada = Prioridades[prioridade];
    }
    
    return tarefa_selecionada;
}

/*********************************************/
void CriaTarefa(tarefa_t p, const char * nome,
stackptr_t pilha, uint16_t tamanho, prioridade_t prioridade)
{
    if (tamanho < TAM_MINIMO_PILHA) {
        return;
    }
    
    pilha = CriaContexto(p, pilha + tamanho);
    
    /* incrementa o numero de tarefas instaladas */
    numero_tarefas++;

    /* guarda os dados no bloco de controle da tarefa (TCB) */
    TCB[numero_tarefas].nome = nome;
    TCB[numero_tarefas].stack_pointer = pilha;
    TCB[numero_tarefas].estado = PRONTA;
    TCB[numero_tarefas].prioridade = prioridade;
    TCB[numero_tarefas].tempo_espera = 0;
      
    /* guarda o numero da tarefa no vetor de prioridades */
    Prioridades[prioridade] = numero_tarefas;
}

/* Servicos do gerenciador de tarefas */
void TarefaSuspende(uint8_t id_tarefa)
{
    REG_ATOMICA_INICIO();
    TCB[id_tarefa].estado = ESPERA; /* tarefa em espera */
    TrocaContexto(); 		   		
    REG_ATOMICA_FIM();
}

void TarefaContinua(uint8_t id_tarefa)
{
    REG_ATOMICA_INICIO();
    TCB[id_tarefa].estado = PRONTA; /* tarefa pronta */
    TrocaContexto(); 		   		
    REG_ATOMICA_FIM();
}

void TarefaEspera(tick_t qtas_marcas)
{
    if (qtas_marcas > 0) {
        REG_ATOMICA_INICIO();
        TCB[tarefa_atual].tempo_espera = qtas_marcas;
        TCB[tarefa_atual].estado = ESPERA;
        TrocaContexto();
        REG_ATOMICA_FIM();
    }
}

/* Tarefa ociosa */
void tarefa_ociosa(void)
{
    for (;;) {		
        REG_ATOMICA_INICIO();
        TrocaContexto();
        REG_ATOMICA_FIM();
    }
}

void IniciaMultitarefas(void)
{
    tarefa_atual = escalonador();
    ponteiro_de_pilha = TCB[tarefa_atual].stack_pointer;
    SP = ponteiro_de_pilha;  /* SP agora é stackptr_t */
    GERA_INTERRUPCAO_SW();
}

void TrocaContextoDasTarefas(void)
{
    /* guarda o valor antigo do stack pointer */
    TCB[tarefa_atual].stack_pointer = SP;
        
    /* executa o escalonador */
    proxima_tarefa = escalonador();
        
    /* seleciona a nova tarefa */
    tarefa_atual = proxima_tarefa;
        
    /* coloca um novo valor no stack pointer */
    ponteiro_de_pilha = TCB[tarefa_atual].stack_pointer;
    SP = ponteiro_de_pilha;
}

void ExecutaMarcaDeTempo(void)
{
    uint8_t tarefa;
        
    ++contador_marcas;
    
    /* decrementa tempo de espera das tarefas */
    for (tarefa = numero_tarefas; tarefa > 0; tarefa--) { 
        if (TCB[tarefa].tempo_espera > 0) {	
            TCB[tarefa].tempo_espera--;
            if (TCB[tarefa].tempo_espera == 0) {
                TCB[tarefa].estado = PRONTA;	        				
            }
        }
    }
}

/* Servicos de semaforos */
void SemaforoAguarda(semaforo_t* sem)
{
    REG_ATOMICA_INICIO();
    
    if (sem->contador > 0) {
        sem->contador--;
    } else {
        TCB[tarefa_atual].estado = ESPERA;
        sem->tarefaEsperando = tarefa_atual;
        TROCA_CONTEXTO();
    }
    
    REG_ATOMICA_FIM();
}

void SemaforoLibera(semaforo_t* sem)
{
    REG_ATOMICA_INICIO();
    
    if (sem->tarefaEsperando > 0) {
        TCB[sem->tarefaEsperando].estado = PRONTA;
        sem->tarefaEsperando = 0;
    } else {
        sem->contador++;
    }
    TROCA_CONTEXTO();
    
    REG_ATOMICA_FIM();
}
