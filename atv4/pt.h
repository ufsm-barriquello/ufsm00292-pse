/*
 * pt.h - protothreads minimas, no estilo das protothreads de Adam Dunkels
 *        (http://dunkels.com/adam/pt/).
 *
 * Uma protothread e uma funcao que consegue parar no meio e continuar dali
 * na proxima chamada. Isso e feito guardando o numero da linha onde parou
 * (campo lc) e voltando pra ela com um switch.
 *
 * Regra importante: variavel local de protothread que precisa sobreviver a
 * uma espera tem que ser declarada static, porque a pilha e perdida entre
 * uma chamada e outra.
 */
#ifndef PT_H
#define PT_H

#define PT_ESPERANDO 0
#define PT_ENCERRADA 1

struct pt {
    unsigned lc;                 /* linha onde a protothread parou */
};

#define PT_INIT(pt)   ((pt)->lc = 0)

#define PT_BEGIN(pt)  switch ((pt)->lc) { case 0:

#define PT_END(pt)    } (pt)->lc = 0; return PT_ENCERRADA

/* Para a protothread ate a condicao ser verdadeira. */
#define PT_WAIT_UNTIL(pt, condicao)      \
    (pt)->lc = __LINE__;                 \
    case __LINE__:                       \
        if (!(condicao)) return PT_ESPERANDO

/* Devolve o controle uma vez e continua na proxima chamada. */
#define PT_YIELD(pt) PT_WAIT_UNTIL(pt, 1)

/* Encerra a protothread agora. */
#define PT_EXIT(pt) do { (pt)->lc = 0; return PT_ENCERRADA; } while (0)

#endif
