# Atividade 2 - tarefa periódica em RTOS

## Implementação

A tarefa `tarefa_periodica_100ms` incrementa a variável global
`execucoes_tarefa_periodica` e chama `TarefaEspera(100)`. Como a marca de tempo
do RTOS está configurada para 1 kHz, 100 ticks correspondem a 100 ms. A variável
pode ser acompanhada pela janela de inspeção da IDE e deve aumentar cerca de dez
vezes por segundo.

A tarefa foi adicionada aos projetos IAR EWARM, SAM D21 e SAM R21 com prioridade
3. O limite `NUMERO_DE_TAREFAS` passou a ser 4, considerando as três tarefas da
aplicação e a tarefa ociosa.

## Como executar nos dois modos

O modo é selecionado pela macro `RTOS_MODO_PREEMPTIVO`, declarada em `rtos.h`:

- `RTOS_MODO_PREEMPTIVO = 0`: modo cooperativo (padrão);
- `RTOS_MODO_PREEMPTIVO = 1`: modo preemptivo.

Para testar o modo preemptivo sem editar o cabeçalho, também é possível definir
`RTOS_MODO_PREEMPTIVO=1` nas opções de símbolos do pré-processador da IDE.

Em cada modo, execute o projeto por alguns segundos e acompanhe
`execucoes_tarefa_periodica` no depurador. O incremento confirma as ativações a
cada 100 ms.

## Comparação

No modo cooperativo, uma tarefa continua usando a CPU até chamar explicitamente
um serviço que provoque a troca de contexto, como `TarefaEspera`. A implementação
é simples e tem menor sobrecarga, porém uma tarefa que não cede a CPU pode atrasar
as demais e aumentar a variação do período observado.

No modo preemptivo, a interrupção de `SysTick` solicita uma troca de contexto a
cada tick. Quando a tarefa periódica volta ao estado pronto, sua prioridade 3
permite que ela interrompa tarefas de prioridade menor. Isso melhora o tempo de
resposta e deixa a execução mais próxima do período de 100 ms, ao custo de mais
trocas de contexto e maior sobrecarga.

Nos dois modos, `TarefaEspera(100)` bloqueia a tarefa sem ocupar a CPU durante a
espera. A diferença principal é quem decide o instante da troca: a própria tarefa
no cooperativo ou o temporizador do sistema no preemptivo.
