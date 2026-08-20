# Tarefa 01 - Buffer Circular (FIFO) com TDD

Módulo em C que implementa uma fila **FIFO** (*first-in, first-out*) sobre um
buffer circular de memória estática, desenvolvido com **TDD**.

---

## O problema

Uma fila FIFO feita sobre um array comum tem um defeito grave: a cada remoção do
primeiro elemento, todos os outros precisam ser deslocados uma posição para a
esquerda. Isso custa **O(n)** por remoção, inviável em embarcados, onde CPU e
memória são escassas.

## Como foi resolvido

Em vez de mover os dados, movem-se **dois índices**. Os dados ficam parados na
memória; o que "anda" são os ponteiros de leitura e escrita, que ao chegarem ao
fim do array **dão a volta** (*wrap-around*) e voltam para a posição 0. O array
de tamanho fixo passa a se comportar como um anel.

Três variáveis de controle:

| Campo   | Significado                                        |
| ------- | -------------------------------------------------- |
| `head`  | índice onde o próximo `push` **escreve**            |
| `tail`  | índice de onde o próximo `pop` **lê**               |
| `count` | quantos elementos existem agora                     |

A circularidade vem da **aritmética modular**:

```c
b->head = (b->head + 1) % BUFFER_SIZE;   // 0 → 1 → 2 → 3 → 4 → 0 → 1 → ...
```

Resultado: **push e pop em O(1)**, memória constante e conhecida em tempo de
compilação, sem `malloc` e sem nunca acessar fora dos limites do array.

**Por que existe o `count`:** comparar `head` com `tail` é ambíguo, `head == tail` acontece tanto no buffer vazio quanto no cheio. Um contador
explícito elimina a ambiguidade (`count == 0` é vazio, `count == BUFFER_SIZE` é
cheio) sem desperdiçar uma posição do array, que é o custo da solução
alternativa clássica.

---

## Passo a passo visual

Estado inicial, após `buffer_init` — `head = tail = 0`:

```
índice:   0     1     2     3     4
        [ -  ][ -  ][ -  ][ -  ][ -  ]
          ^
        head/tail          count = 0  (vazio)
```

`push(10)`, `push(20)`, `push(30)` — só o **head** andou:

```
índice:   0     1     2     3     4
        [ 10 ][ 20 ][ 30 ][ -  ][ -  ]
          ^                ^
        tail             head          count = 3
```

`pop()` → retorna `10`. Só o **tail** andou. O `10` continua fisicamente na
memória, mas está logicamente fora da fila porque o `tail` já passou dele, é por isso que "remover" custa O(1):

```
índice:   0     1     2     3     4
        [ 10 ][ 20 ][ 30 ][ -  ][ -  ]
                ^          ^
              tail       head          count = 2
```

`push(40)`, `push(50)`, `push(60)` — o **head dá a volta**, reaproveitando a
posição 0 que o `tail` já liberou:

```
índice:   0     1     2     3     4
        [ 60 ][ 20 ][ 30 ][ 40 ][ 50 ]
          ^     ^
        head   tail                    count = 5  (cheio)
```

Aqui `head == tail == 1` com o buffer **cheio**; no estado inicial `head == tail`
também, mas **vazio**. É exatamente essa ambiguidade que o `count` resolve.