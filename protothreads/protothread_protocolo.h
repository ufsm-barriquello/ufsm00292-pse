#ifndef PROTOTHREAD_PROTOCOLO_H
#define PROTOTHREAD_PROTOCOLO_H

/* --- Macros de Protothreads (Continuação Local) --- */
struct pt { unsigned short lc; };
#define PT_INIT(pt) (pt)->lc = 0
#define PT_BEGIN(pt) switch((pt)->lc) { case 0:
#define PT_WAIT_UNTIL(pt, c) (pt)->lc = __LINE__; case __LINE__: if(!(c)) return 1
#define PT_EXIT(pt) (pt)->lc = 0; return 0
#define PT_END(pt) } (pt)->lc = 0; return 0

/* --- Definições do Protocolo --- */
#define STX 0x02
#define ETX 0x03

/* --- Variáveis Globais (Contexto do Sistema) --- */
extern unsigned char canal_comunicacao;
extern int tem_dado_no_canal;
extern int tx_concluido;
extern int pacote_recebido_com_sucesso;

extern struct pt pt_tx, pt_rx;
extern unsigned char payload_tx[256], buffer_rx[256];
extern int qtd_tx, qtd_rx;
extern int idx_tx, idx_rx;
extern unsigned char chk_tx, chk_rx;

void reset_sistema(void);
int thread_transmissora(struct pt *pt);
int thread_receptora(struct pt *pt);

#endif