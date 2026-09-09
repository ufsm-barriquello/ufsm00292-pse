#include "protocolo.h"

void rundata_reset(Rundata *r) {
    if (!r) return;
    r->chksum = 0;
    r->n = 0;
    r->qtd = 0;
}

/* Inicialização do contexto event-driven */
void protocolo_pt_init(struct protocolo_pt *ctx) {
    if (!ctx) return;
    PT_INIT(&ctx->pt);
    ctx->estado = STATE_START;
    rundata_reset(&ctx->data);
    ctx->byte = 0;
    ctx->byte_ready = 0;
    ctx->frame_complete = 0;
    ctx->frame_error = 0;
    ctx->qhead = 0;
    ctx->qtail = 0;
    ctx->qcount = 0;
}

void protocolo_pt_input(struct protocolo_pt *ctx, uint8_t byte) {
    if (!ctx) return;
    if (ctx->qcount < sizeof(ctx->qbuf)) {
        ctx->qbuf[ctx->qtail] = byte;
        ctx->qtail = (ctx->qtail + 1) % sizeof(ctx->qbuf);
        ctx->qcount++;
    }
    ctx->byte = byte;
    ctx->byte_ready = 1;
}

static inline uint8_t pt_dequeue(struct protocolo_pt *ctx) {
    uint8_t b = ctx->qbuf[ctx->qhead];
    ctx->qhead = (ctx->qhead + 1) % sizeof(ctx->qbuf);
    ctx->qcount--;
    ctx->byte = b;
    ctx->byte_ready = ctx->qcount ? 1 : 0;
    return b;
}

/* Protothread de passo único: transição de UM byte SEM switch.
 * Linear com PT_YIELD, sem switch (restrição lc-switch.h).
 * Esta é a lógica que substitui o antigo switch de process_byte.
 */
PT_THREAD(protocolo_thread_step(struct pt *pt, struct protocolo_pt *ctx, uint8_t byte)) {
    PT_BEGIN(pt);
    if (ctx->estado == STATE_START) {
        if (byte == PROTOCOL_STX) {
            rundata_reset(&ctx->data);
            ctx->estado = STATE_QTD;
        } else {
            ctx->estado = STATE_START;
        }
    } else if (ctx->estado == STATE_QTD) {
        ctx->data.qtd = byte;
        ctx->data.n = 0;
        ctx->data.chksum = 0;
        if (ctx->data.qtd == 0) {
            ctx->estado = STATE_CHKSUM;
        } else {
            ctx->estado = STATE_DATA;
        }
    } else if (ctx->estado == STATE_DATA) {
        ctx->data.chksum ^= byte;
        ctx->data.n++;
        if (ctx->data.n >= ctx->data.qtd) {
            ctx->estado = STATE_CHKSUM;
        } else {
            ctx->estado = STATE_DATA;
        }
    } else if (ctx->estado == STATE_CHKSUM) {
        if (byte == ctx->data.chksum) {
            ctx->estado = STATE_END;
        } else {
            rundata_reset(&ctx->data);
            ctx->estado = STATE_START;
        }
    } else if (ctx->estado == STATE_END) {
        rundata_reset(&ctx->data);
        if (byte == PROTOCOL_ETX) {
            ctx->estado = STATE_START;
        } else if (byte == PROTOCOL_STX) {
            ctx->estado = STATE_QTD;
        } else {
            ctx->estado = STATE_START;
        }
    } else {
        rundata_reset(&ctx->data);
        ctx->estado = STATE_START;
    }
    ctx->byte = byte;
    PT_YIELD(pt);
    PT_END(pt);
}

/* Wrapper síncrono que usa protothread_step para processar um byte.
 * Esta função substitui o antigo process_byte (que usava switch).
 */
Estado protocolo_pt_step(struct protocolo_pt *ctx, uint8_t byte) {
    if (!ctx) return STATE_START;
    struct pt pt;
    PT_INIT(&pt);
    PT_SCHEDULE(protocolo_thread_step(&pt, ctx, byte));
    /* protocolo_thread_step faz PT_YIELD então precisa de segundo schedule para completar? */
    PT_SCHEDULE(protocolo_thread_step(&pt, ctx, byte));
    return ctx->estado;
}

size_t protocolo_pt_feed(struct protocolo_pt *ctx, const uint8_t *buf, size_t len) {
    if (!ctx || !buf) return 0;
    for (size_t i = 0; i < len; i++) {
        protocolo_pt_input(ctx, buf[i]);
        PT_SCHEDULE(protocolo_thread(&ctx->pt, ctx));
    }
    return len;
}

/* Compatibilidade: antigo process_byte agora DELEGA para protothread.
 * Não contém switch; usa protocolo_thread_step internamente.
 */
Estado process_byte(uint8_t entrada, Estado atual, Rundata *armazenamento) {
    if (!armazenamento) return STATE_START;
    struct protocolo_pt ctx;
    struct pt pt;
    protocolo_pt_init(&ctx);
    ctx.estado = atual;
    ctx.data = *armazenamento;
    PT_INIT(&pt);
    PT_SCHEDULE(protocolo_thread_step(&pt, &ctx, entrada));
    PT_SCHEDULE(protocolo_thread_step(&pt, &ctx, entrada));
    *armazenamento = ctx.data;
    return ctx.estado;
}

/* Protothread principal event-driven com fila. */
PT_THREAD(protocolo_thread(struct pt *pt, struct protocolo_pt *ctx)) {
    uint8_t cur;
    uint8_t q;
    uint8_t chk;
    uint8_t etx;
    PT_BEGIN(pt);
    while (1) {
        ctx->estado = STATE_START;
        rundata_reset(&ctx->data);
        ctx->frame_complete = 0;
        PT_WAIT_UNTIL(pt, ctx->qcount > 0);
        while (ctx->qbuf[ctx->qhead] != PROTOCOL_STX) {
            cur = pt_dequeue(ctx);
            (void)cur;
            ctx->frame_error = 0;
            PT_YIELD(pt);
            PT_WAIT_UNTIL(pt, ctx->qcount > 0);
        }
        cur = pt_dequeue(ctx);
        (void)cur;
        ctx->estado = STATE_QTD;
        ctx->frame_error = 0;
        PT_YIELD(pt);
        PT_WAIT_UNTIL(pt, ctx->qcount > 0);
        q = pt_dequeue(ctx);
        ctx->data.qtd = q;
        ctx->data.n = 0;
        ctx->data.chksum = 0;
        if (ctx->data.qtd == 0) {
            ctx->estado = STATE_CHKSUM;
        } else {
            ctx->estado = STATE_DATA;
            while (ctx->data.n < ctx->data.qtd) {
                PT_WAIT_UNTIL(pt, ctx->qcount > 0);
                cur = pt_dequeue(ctx);
                ctx->data.chksum ^= cur;
                ctx->data.n++;
                if (ctx->data.n < ctx->data.qtd) PT_YIELD(pt);
            }
            ctx->estado = STATE_CHKSUM;
            PT_YIELD(pt);
        }
        PT_WAIT_UNTIL(pt, ctx->qcount > 0);
        chk = pt_dequeue(ctx);
        if (chk != ctx->data.chksum) {
            rundata_reset(&ctx->data);
            ctx->estado = STATE_START;
            ctx->frame_error = 1;
            PT_YIELD(pt);
            continue;
        }
        ctx->estado = STATE_END;
        ctx->frame_error = 0;
        PT_YIELD(pt);
        PT_WAIT_UNTIL(pt, ctx->qcount > 0);
        etx = pt_dequeue(ctx);
        rundata_reset(&ctx->data);
        if (etx == PROTOCOL_ETX) {
            ctx->estado = STATE_START;
            ctx->frame_complete = 1;
            PT_YIELD(pt);
        } else if (etx == PROTOCOL_STX) {
            if (ctx->qcount < sizeof(ctx->qbuf)) {
                ctx->qbuf[ctx->qtail] = PROTOCOL_STX;
                ctx->qtail = (ctx->qtail + 1) % sizeof(ctx->qbuf);
                ctx->qcount++;
                ctx->byte = PROTOCOL_STX;
                ctx->byte_ready = 1;
            }
            ctx->estado = STATE_START;
            PT_YIELD(pt);
        } else {
            ctx->estado = STATE_START;
            PT_YIELD(pt);
        }
    }
    PT_END(pt);
}
