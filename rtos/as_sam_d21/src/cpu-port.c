/*
 * cpu_port.c
 *
 * Author: Carlos H. Barriquello
 */ 

#include <asf.h>
#include "cpu-port.h"
#include "rtos.h"

stackptr_t CriaContexto(tarefa_t endereco_tarefa, stackptr_t ptr_pilha)
{
	#define INITIAL_XPSR		0x01000000
	
	uint32_t reg_val;
	*(--ptr_pilha) = INITIAL_XPSR;     /* xPSR */
	*(--ptr_pilha) = (uint32_t)endereco_tarefa;  /* R15 */
	*(--ptr_pilha) = 0x00;				   /* R14 */
	
	*(--ptr_pilha) = 0x12;			   /* R12 */
	
	for(reg_val = 3; reg_val > 0; reg_val--)
	{
		*(--ptr_pilha) = reg_val;  /* R3, R2, R1 */
	}
	
	*(--ptr_pilha) = 0;		   /* R0 */
	
	for(reg_val = 7; reg_val >= 4; reg_val--)
	{
		*(--ptr_pilha) = reg_val;  /* R7, R6, R5, R4 */
	}
	
	for(reg_val = 11; reg_val >= 8; reg_val--)
	{
		*(--ptr_pilha) = reg_val;  /* R11, R10, R9, R8 */
	}
	
	return ptr_pilha;

	
}

/* Codigo dependente de hardware usado para 
 * configuracao da marca de tempo do sistema multitarefas */
void ConfiguraMarcaTempo(void)
{   
	
	    uint32_t cpu_clock_hz = 48000000UL; //system_cpu_clock_get_hz();
		uint16_t valor_comparador = cpu_clock_hz/cfg_MARCA_TEMPO_HZ; //(cfg_CPU_CLOCK_HZ / cfg_MARCA_TEMPO_HZ);
		
		*(NVIC_SYSTICK_CTRL) = 0;						// Desabilita SysTick Timer
		*(NVIC_SYSTICK_LOAD) = valor_comparador - 1;	// Configura a contagem
		*(NVIC_SYSTICK_CTRL) = NVIC_SYSTICK_CLK | NVIC_SYSTICK_INT | NVIC_SYSTICK_ENABLE;  // Inicia
}

/* rotinas de interrupcao necessarias */
__attribute__ ((naked)) void SVC_Handler(void)
{
	/* Make PendSV and SysTick the lowest priority interrupts. */
	*(NVIC_SYSPRI3) |= NVIC_PENDSV_PRI;
	*(NVIC_SYSPRI3) |= NVIC_SYSTICK_PRI;
	RESTAURA_SP(SP);
	RESTAURA_CONTEXTO();
	RESTAURA_ISR();
}

__attribute__ ((naked)) void PendSV_Handler(void)
{
	
	SALVA_ISR();
	SALVA_CONTEXTO();
	SALVA_SP(SP);
	
	Clear_PendSV();
	
    TrocaContextoDasTarefas();
	
	RESTAURA_SP(SP);
	RESTAURA_CONTEXTO();
	RESTAURA_ISR();
	
}

/* Codigo dependente de hardware usado para 
   realizar a marca de tempo do sistema multitarefas - interrupcao */
void SysTick_Handler(void)
{	
    
	/********************************************************************************
    * Diferença na Execução da Tarefa: Cooperativo vs. Preemptivo
    *
    * MODO COOPERATIVO:
    * Uma tarefa executa até que ela voluntariamente ceda o controle do processador
    * (ex: chamando TarefaEspera). Uma tarefa mal comportada em um loop infinito
    * pode travar todo o sistema, pois nunca liberará o processador para as outras.
    * A execução de tarefas periódicas pode sofrer atrasos (jitter).
    * 
    * Exemplo: A tarefa_9 (P3) inicia e monopoliza o processador até decidir parar.
    * Somente então, as tarefas de prioridade 2 (tarefa_2, tarefa_periodica) rodam,
    * uma de cada vez, até que também cedam sua vez. A tarefa_1 (P1) só executa
    * se todas as de maior prioridade estiverem esperando. A execução depende da
    * "boa vontade" de cada tarefa.
    *
    * MODO PREEMPTIVO:
    * O RTOS pode forçar a interrupção (preempção) de uma tarefa em execução
    * para dar lugar a uma tarefa de maior prioridade que acabou de ficar pronta.
    * Isso garante que tarefas importantes e de alta prioridade respondam rapidamente,
    * tornando o sistema mais robusto e o tempo de execução mais previsível.
    * 
    * A tarefa de maior prioridade que estiver pronta SEMPRE está no controle.
    * Exemplo: Se a tarefa_1 (P1) está rodando e a tarefa_periodica (P2) acorda,
    * o sistema INTERROMPE a tarefa_1 à força para rodar a tarefa_periodica
    * imediatamente. A ordem é ditada pela prioridade para garantir que tarefas
    * importantes não atrasem.
    ********************************************************************************/
        
	 ExecutaMarcaDeTempo(); 
     //Para executar a nova tarefa periodica no modo cooperativo e preemptivo, primeiro foi testada em modo cooperativo com a linha TrocaContexto comentada.
     //Para o teste de execução da nova tarefa em modo preemptivo a linha foi descomentada.
      
	 TrocaContexto();
}

void HardFault_Handler(void)
{
	
	while(1)
	{
		
		
	}
}



