# Exercício 2 - Análise: Modo Cooperativo vs Preemptivo

## Implementação
- Tarefa periódica criada com período de 100ms
- Testado em ambos os modos de escalonamento

## Modo Cooperativo (Original)
- Troca de contexto apenas quando tarefa chama explicitamente funções como TarefaEspera()
- Controle total das tarefas sobre quando ceder o processador
- Menor overhead de sistema
- Risco: se tarefa trava em loop, sistema para

## Modo Preemptivo (Modificado)  
- SysTick força troca de contexto a cada 1ms (independente da vontade da tarefa)
- Sistema garante que nenhuma tarefa monopolize o processador
- Maior overhead devido às interrupções frequentes
- Mais robusto contra travamentos

## Diferenças Observadas
- Preemptivo: melhor responsividade geral do sistema
- Cooperativo: mais previsível, menor latência para tarefas críticas
- Preemptivo: previne starvation de tarefas de baixa prioridade
