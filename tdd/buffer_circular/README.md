# Exercício 3 — Buffer circular com TDD

Solução do Exercício 3 da aula "Primeiro projeto TDD" (UFSM00292 — Projeto de
Sistemas Embarcados): implementação de um módulo de manipulação de *buffer
circular* desenvolvido pela metodologia TDD (*test driven development*).

Um buffer circular é uma forma eficiente de implementar uma fila FIFO
(*first-in, first-out*) usando um espaço de memória limitado.

## Arquivos

| Arquivo | Conteúdo |
| --- | --- |
| `buffer_circular.h` | Interface do módulo |
| `buffer_circular.c` | Implementação |
| `testes_buffer_circular.c` | Os 23 testes unitários e o programa de testes |
| `Makefile` | Compilação e execução dos testes |

## Como compilar e executar os testes

```
make test
```

Ou, sem `make`:

```
gcc -std=c99 -Wall -Wextra -pedantic -o testes testes_buffer_circular.c buffer_circular.c
./testes
```

Saída esperada:

```
TODOS OS TESTES PASSARAM
Testes executados: 23
```

O programa de testes usa as macros `verifica` e `executa_teste` apresentadas
em aula e retorna código de saída diferente de zero quando algum teste falha,
o que permite usá-lo em integração contínua.

## Interface do módulo

```c
int    bc_inicializa(buffer_circular_t *b, uint8_t *area, size_t tamanho);
int    bc_limpa(buffer_circular_t *b);
int    bc_escreve(buffer_circular_t *b, uint8_t dado);
int    bc_le(buffer_circular_t *b, uint8_t *dado);
int    bc_vazio(const buffer_circular_t *b);
int    bc_cheio(const buffer_circular_t *b);
size_t bc_ocupacao(const buffer_circular_t *b);
size_t bc_capacidade(const buffer_circular_t *b);
```

Códigos de retorno: `BC_OK` (0), `BC_ERRO_PARAMETRO` (-1), `BC_ERRO_CHEIO`
(-2) e `BC_ERRO_VAZIO` (-3).

### Decisões de projeto

- **Sem alocação dinâmica.** A área de memória é fornecida pelo usuário na
  inicialização, o que é adequado a sistemas embarcados com memória limitada
  e evita a dependência de `malloc()`.
- **Estado representado por `inicio` + `contador`**, em vez de dois índices
  (leitura e escrita). Com dois índices, as condições "cheio" e "vazio" ficam
  ambíguas (ambas dão `inicio == fim`), o que obriga a sacrificar uma posição
  do buffer. Com o contador, toda a capacidade é utilizável.
- **Escrita em buffer cheio é rejeitada** (`BC_ERRO_CHEIO`) em vez de
  sobrescrever o dado mais antigo. Para uma fila FIFO de comunicação, perder
  silenciosamente o dado mais antigo esconde o estouro do buffer; o erro
  explícito permite que o chamador trate a situação.
- **Ponteiros nulos são tratados**, e não assumidos válidos: em código
  embarcado uma desreferência inválida trava o sistema.

## 1. Lista dos testes que devem ser feitos

Levantamento feito **antes** da implementação, conforme pedido no item 1 do
exercício. Os testes foram agrupados por ciclo TDD:

**Criação e estado inicial**

1. `bc_inicializa()` retorna `BC_OK` com parâmetros válidos
2. Um buffer recém-inicializado está vazio
3. Um buffer recém-inicializado não está cheio
4. `bc_capacidade()` retorna o tamanho informado
5. `bc_ocupacao()` inicial é zero

**Leitura e escrita**

6. Ler de um buffer vazio retorna `BC_ERRO_VAZIO` e não altera o destino
7. `bc_escreve()` retorna `BC_OK`
8. Após escrever, o buffer não está mais vazio e a ocupação é 1
9. `bc_le()` devolve o dado que foi escrito
10. A ordem de leitura é FIFO (escreve 1, 2, 3 → lê 1, 2, 3)
11. Após ler todos os dados, o buffer volta a ficar vazio

**Buffer cheio**

12. O buffer fica cheio ao atingir a capacidade
13. Escrever em buffer cheio retorna `BC_ERRO_CHEIO`
14. A escrita rejeitada não sobrescreve os dados armazenados

**Comportamento circular**

15. Os índices dão a volta ao final da área e a ordem FIFO é mantida
16. O uso contínuo por várias voltas mantém a FIFO correta

**Descarte e casos limite**

17. `bc_limpa()` esvazia o buffer
18. O buffer continua utilizável após `bc_limpa()`
19. Um buffer de uma única posição funciona corretamente

**Robustez**

20. `bc_inicializa()` rejeita buffer nulo, área nula e tamanho zero
21. `bc_escreve()` rejeita buffer nulo
22. `bc_le()` rejeita buffer nulo e destino nulo, sem consumir dados
23. As funções de consulta não acessam memória inválida

## 2. Relatório dos testes incrementais

Cada ciclo TDD corresponde a um *commit* deste repositório, na ordem
escrita do teste → falha (vermelho) → código mínimo → aprovação (verde).

### Ciclo 1 — criação e estado inicial (testes 1–2)

Vermelho — o módulo ainda não existia:

```
gcc.exe: error: buffer_circular.c: No such file or directory
```

Verde, após criar `buffer_circular.h`/`.c` com `bc_inicializa()` e `bc_vazio()`:

```
TODOS OS TESTES PASSARAM
Testes executados: 2
```

### Ciclo 2 — capacidade, ocupação e leitura de buffer vazio (testes 3–6)

Vermelho — funções ainda não declaradas:

```
testes_buffer_circular.c:77:14: warning: implicit declaration of function 'bc_cheio'
collect2.exe: error: ld returned 1 exit status
```

Verde, após implementar `bc_cheio()`, `bc_capacidade()`, `bc_ocupacao()` e
`bc_le()` (que retorna `BC_ERRO_VAZIO` quando não há dados):

```
TODOS OS TESTES PASSARAM
Testes executados: 6
```

### Ciclo 3 — escrita, leitura do dado e ordem FIFO (testes 7–11)

Vermelho:

```
testes_buffer_circular.c:128:14: warning: implicit declaration of function 'bc_escreve'
collect2.exe: error: ld returned 1 exit status
```

Verde, após implementar `bc_escreve()` gravando na posição
`(inicio + contador) % tamanho`:

```
TODOS OS TESTES PASSARAM
Testes executados: 11
```

### Ciclo 4 — buffer cheio e proteção contra estouro (testes 12–14)

Vermelho em duas etapas. Primeiro o código de erro nem existia:

```
testes_buffer_circular.c:236:38: error: 'BC_ERRO_CHEIO' undeclared
```

Depois de definir `BC_ERRO_CHEIO`, a falha passou a ser de comportamento —
`bc_escreve()` aceitava escrever em um buffer já cheio:

```
erro: escrever em buffer cheio deveria retornar BC_ERRO_CHEIO
Testes executados: 13
```

Verde, após adicionar a verificação `bc_cheio()` no início de `bc_escreve()`:

```
TODOS OS TESTES PASSARAM
Testes executados: 14
```

### Ciclo 5 — comportamento circular / *wrap-around* (testes 15–16)

Este ciclo **passou sem falha inicial**: a aritmética modular introduzida nos
ciclos 2 e 3 já tratava corretamente a volta dos índices. Os testes foram
mantidos como testes de regressão, pois protegem justamente a característica
que dá nome ao módulo.

```
TODOS OS TESTES PASSARAM
Testes executados: 16
```

### Ciclo 6 — `bc_limpa()` e caso limite (testes 17–19)

Vermelho — `bc_limpa()` ainda não existia:

```
testes_buffer_circular.c:346:14: warning: implicit declaration of function 'bc_limpa'
collect2.exe: error: ld returned 1 exit status
```

Verde, após implementar `bc_limpa()`. O teste do buffer de uma única posição
passou junto, confirmando que o caso limite `tamanho == 1` já era tratado:

```
TODOS OS TESTES PASSARAM
Testes executados: 19
```

### Ciclo 7 — robustez contra parâmetros inválidos (testes 20–23)

Vermelho da forma mais evidente possível — o programa de testes foi encerrado
pelo sistema operacional ao desreferenciar um ponteiro nulo:

```
Segmentation fault
```

Verde, após verificar os ponteiros em todas as funções públicas:

```
TODOS OS TESTES PASSARAM
Testes executados: 23
```

**Refatoração** (etapa 3 do ciclo TDD), feita com todos os testes passando e
confirmada em seguida pela mesma bateria: extração da função auxiliar
`bc_indice_escrita()`, que dá nome ao cálculo `(inicio + contador) % tamanho`
repetido no módulo, e reorganização das funções por responsabilidade. Nenhum
teste precisou ser alterado — que é exatamente o que a rede de testes deve
permitir.

## Resumo

| Ciclo | Testes | Total acumulado | Falha inicial |
| --- | --- | --- | --- |
| 1 | 2 | 2 | compilação (módulo inexistente) |
| 2 | 4 | 6 | ligação (funções inexistentes) |
| 3 | 5 | 11 | ligação (função inexistente) |
| 4 | 3 | 14 | compilação e, depois, comportamento |
| 5 | 2 | 16 | nenhuma (teste de regressão) |
| 6 | 3 | 19 | ligação (função inexistente) |
| 7 | 4 | 23 | falha de segmentação |

Ambiente de desenvolvimento: GCC 6.3.0 (MinGW), Windows 10.
