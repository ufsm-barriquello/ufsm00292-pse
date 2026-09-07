# Tarefa periódica de 100 ms: cooperativo x preemptivo

A tarefa está em `rtos/as_sam_d21/src/main.c` (`tarefa_periodica`, prioridade 4).
Ela chama `TarefaEspera(100)` e, como `cfg_MARCA_TEMPO_HZ` vale 1000, cada marca
de tempo dura 1 ms e o período pedido é de 100 ms.

## Como alternar entre os dois modos

Em `rtos/as_sam_d21/src/rtos.h`:

```c
#define MODO_PREEMPTIVO     0   /* 0 = cooperativo, 1 = preemptivo */
```

A macro controla a única diferença de código entre os dois modos, no
`SysTick_Handler` de `cpu-port.c`:

```c
ExecutaMarcaDeTempo();
#if MODO_PREEMPTIVO
 TrocaContexto();   /* preemptivo: a marca de tempo já força a troca de contexto */
#endif
```

## Como medir

A tarefa inverte o LED da placa a cada execução, então o LED pisca a 5 Hz
(100 ms aceso, 100 ms apagado). Medindo o pino do LED no osciloscópio, o
semiperíodo é o período da tarefa e a variação entre bordas é o jitter.

## Diferenças

**Cooperativo.** O `SysTick` só atualiza os contadores de espera das tarefas
(`ExecutaMarcaDeTempo`) e volta para a tarefa que estava executando. Quando o
tempo de espera da tarefa periódica chega a zero, ela fica pronta, mas só entra
em execução quando alguma outra tarefa ceder a CPU — isto é, quando alguém
chamar `TarefaEspera`, `TarefaSuspende`, `TarefaContinua`, um serviço de
semáforo, ou quando a tarefa ociosa executar. O período fica, portanto, igual ou
maior que 100 ms, e o atraso depende de quanto tempo as outras tarefas passam
sem ceder. No caso extremo de uma tarefa entrar em laço sem ceder a CPU, a
tarefa periódica simplesmente não executa, mesmo tendo prioridade maior.

**Preemptivo.** O `SysTick` solicita a troca de contexto a cada marca de tempo.
Assim que o contador de espera zera, o escalonador já roda na própria
interrupção e escolhe a tarefa periódica, que tem a maior prioridade. O atraso
em relação aos 100 ms fica limitado a cerca de uma marca de tempo (1 ms), sem
depender do comportamento das outras tarefas.

**Custo do preemptivo.** Passa a existir uma troca de contexto potencial a cada
1 ms, o que consome mais CPU e mais pilha. Além disso, qualquer tarefa pode ser
interrompida no meio de uma operação, então o acesso a variáveis compartilhadas
precisa das regiões atômicas (`REG_ATOMICA_INICIO` / `REG_ATOMICA_FIM`), coisa
que no cooperativo era menos crítica porque a troca só acontecia em pontos
escolhidos pelo programador.

**Observação sobre este projeto.** A `tarefa_1` do exemplo chama
`TarefaContinua(2)` dentro do laço, ou seja, cede a CPU o tempo todo. Por isso,
mesmo no modo cooperativo, o desvio medido tende a ficar pequeno neste projeto
específico. Para ver a diferença ficar evidente, basta deixar a `tarefa_1`
rodando um trecho longo sem chamar nenhum serviço do sistema: no cooperativo o
período da tarefa periódica estica junto, no preemptivo ele se mantém.
