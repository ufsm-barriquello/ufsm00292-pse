#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>  
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
 
#define VERMELHO 0
#define AMARELO  1   
#define VERDE    2   

/* Macros de teste */
#define verifica(mensagem, teste) do { if (!(teste)) return mensagem; } while (0)
#define executa_teste(teste) do { char *mensagem = teste(); testes_executados++; \
    if (mensagem) return mensagem; } while (0)

int testes_executados = 0, start=0, end=0;
static char * executa_testes(void);
volatile char ControleMaquinaEstado; 





void ContagemRegressiva(int segundos) {
    for (int i = segundos; i > 0; i--) {
        printf("\rMudando em %2d segundos...", i);
        fflush(stdout);
        #ifdef _WIN32
            Sleep(1000);
        #else
            sleep(1);
        #endif
    }
    printf("\n");
}

void ExecutaLogicaEstadosAutomatico(void) {
    switch(ControleMaquinaEstado) {
        case VERMELHO: ControleMaquinaEstado = AMARELO; break; 
        case AMARELO:  ControleMaquinaEstado = VERDE;   break; 
        case VERDE:    ControleMaquinaEstado = VERMELHO; break; 
    }
}

void ExecutaMaquinaDeEstados(void) {
    ContagemRegressiva(5); // reduzido p/ teste
    ExecutaLogicaEstadosAutomatico();
    switch(ControleMaquinaEstado) {
        case VERMELHO: printf("Estado atual: VERMELHO\n"); break;
        case AMARELO:  printf("Estado atual: AMARELO\n"); break;
        case VERDE:    printf("Estado atual: VERDE\n");   break;
    }
}


/* ---------- Testes ---------- */

static char * teste_vermelho_para_amarelo(void){
    ControleMaquinaEstado = VERMELHO;
    ExecutaLogicaEstadosAutomatico();
    verifica("ERRO: VERMELHO nao foi para AMARELO", ControleMaquinaEstado == AMARELO);
    return 0;
}
static char * teste_amarelo_para_verde(void){
    ControleMaquinaEstado = AMARELO;
    ExecutaLogicaEstadosAutomatico();
    verifica("ERRO: AMARELO nao foi para VERDE", ControleMaquinaEstado == VERDE);
    return 0;
}
static char * teste_verde_para_vermelho(void){
    ControleMaquinaEstado = VERDE;
    ExecutaLogicaEstadosAutomatico();
    verifica("ERRO: VERDE nao foi para VERmelho", ControleMaquinaEstado == VERMELHO);
    return 0;
}

static char * executa_testes(void){
    executa_teste(teste_vermelho_para_amarelo);
    executa_teste(teste_amarelo_para_verde);
    executa_teste(teste_verde_para_vermelho);
    return 0;
}

/* ========= Main ========= */
int main(int argc, char *argv[]) {
    char *resultado = executa_testes();
    if(resultado != 0) {
        printf("%s\n", resultado);
    } else {
        printf("TODOS OS TESTES PASSARAM\n");
    }
    printf("Testes executados: %d\n", testes_executados);

    // Máquina de estados só roda se testes passaram
    ControleMaquinaEstado = VERMELHO; 
    printf("Estado inicial: VERMELHO\n");
    while(1) {
        ExecutaMaquinaDeEstados(); 
    }
    return 0;
}
