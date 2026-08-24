#include "protocolo.h"

/* Cada acao devolve o proximo estado da FSM. */

static estado_t descarta(fsm_t *fsm, uint8_t byte)
{
    (void)fsm;
    (void)byte;
    return AGUARDA_STX;
}

static estado_t inicia(fsm_t *fsm, uint8_t byte)
{
    (void)byte;
    fsm->qtd    = 0;
    fsm->indice = 0;
    fsm->soma   = 0;
    return RECEBE_QTD;
}

static estado_t guarda_qtd(fsm_t *fsm, uint8_t byte)
{
    if (byte == 0) {
        return AGUARDA_STX;          /* mensagem sem dados: descarta */
    }
    fsm->qtd = byte;
    return RECEBE_DADOS;
}

static estado_t guarda_dado(fsm_t *fsm, uint8_t byte)
{
    fsm->dados[fsm->indice] = byte;
    fsm->indice++;
    fsm->soma = (uint8_t)(fsm->soma + byte);
    return (fsm->indice < fsm->qtd) ? RECEBE_DADOS : RECEBE_CHK;
}

static estado_t confere_chk(fsm_t *fsm, uint8_t byte)
{
    return (byte == fsm->soma) ? RECEBE_ETX : AGUARDA_STX;
}

static estado_t finaliza(fsm_t *fsm, uint8_t byte)
{
    (void)byte;
    fsm->pronto = 1;
    return AGUARDA_STX;
}

typedef estado_t (*acao_t)(fsm_t *fsm, uint8_t byte);

/*
 * Tabela de estados/eventos. Nos estados que recebem bytes quaisquer
 * (QTD, DADOS e CHK) a mesma acao vale para os tres eventos, e por isso
 * um 0x02 ou 0x03 no meio dos dados e tratado como dado.
 */
static const acao_t tabela[NUM_ESTADOS][NUM_EVENTOS] = {
    /*                   EV_STX       EV_ETX       EV_DADO      */
    /* AGUARDA_STX  */ { inicia,      descarta,    descarta     },
    /* RECEBE_QTD   */ { guarda_qtd,  guarda_qtd,  guarda_qtd   },
    /* RECEBE_DADOS */ { guarda_dado, guarda_dado, guarda_dado  },
    /* RECEBE_CHK   */ { confere_chk, confere_chk, confere_chk  },
    /* RECEBE_ETX   */ { descarta,    finaliza,    descarta     }
};

void fsm_init(fsm_t *fsm)
{
    fsm->estado = AGUARDA_STX;
    fsm->qtd    = 0;
    fsm->indice = 0;
    fsm->soma   = 0;
    fsm->pronto = 0;
}

void fsm_processa(fsm_t *fsm, uint8_t byte)
{
    evento_t evento;

    if (byte == STX) {
        evento = EV_STX;
    } else if (byte == ETX) {
        evento = EV_ETX;
    } else {
        evento = EV_DADO;
    }

    fsm->pronto = 0;
    fsm->estado = tabela[fsm->estado][evento](fsm, byte);
}
