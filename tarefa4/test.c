#include "protocolo.h"
#include <stdio.h>
#include <string.h>

struct resultado_testes {
    char* mensagem;
    int testes_realizados;
};
typedef char* (*Test)();

/* Helper: processa um byte usando protothread (substitui process_byte) */
static Estado step_via_protothread(uint8_t byte, Estado atual, Rundata *data) {
    struct protocolo_pt ctx;
    protocolo_pt_init(&ctx);
    ctx.estado = atual;
    if (data) ctx.data = *data;
    Estado novo = protocolo_pt_step(&ctx, byte);
    if (data) *data = ctx.data;
    return novo;
}

char* testar_start_1 () {
    Estado inicial = STATE_START;
    Rundata data = {0, 0, 0};
    uint8_t dado = 0x02;
    Estado apos = STATE_QTD;
    if(step_via_protothread(dado, inicial, &data) != apos) {
        return "Estado não mudou de START para QUANTIDADE";
    }
    return 0;
}

char* testar_start_2 () {
    Estado inicial = STATE_START;
    Rundata data = {0, 0, 0};
    uint8_t dado = 0x03;
    Estado apos = STATE_START;
    if(step_via_protothread(dado, inicial, &data) != apos) {
        return "Estado não mudou de START para START";
    }
    return 0;
}

char* testar_qtd_1 () {
    Estado inicial = STATE_QTD;
    Rundata data = {0, 0, 0};
    uint8_t dado = 0x0A;
    Estado apos = STATE_DATA;
    Estado res = step_via_protothread(dado, inicial, &data);
    if(res != apos) {
        return "Estado não mudou de QTD para DADOS";
    }
    if(data.qtd != 10) {
        return "Quantidade correta não gravada";
    }
    return 0;
}

char* testar_qtd_2 () {
    Estado inicial = STATE_QTD;
    Rundata data = {0, 0, 0};
    uint8_t dado = 0x07;
    Estado apos = STATE_DATA;
    Estado res = step_via_protothread(dado, inicial, &data);
    if(res != apos) {
        return "Estado não mudou de QTD para DADOS";
    }
    if(data.qtd != 7) {
        return "Quantidade correta não gravada";
    }
    return 0;
}

char* testar_checksum () {
    // também valida wrapper legado process_byte (que agora usa protothread internamente)
    Estado inicial = STATE_CHKSUM;
    Rundata data = {3, 3, (1^2^4)};
    uint8_t correto = (1^2^4);
    Estado apos = STATE_END;
    if(process_byte(correto, inicial, &data) != apos) {
        return "Estado não mudou de CHKSUM para FIM com checksum correto";
    }
    // checksum incorreto via protothread direta
    inicial = STATE_CHKSUM;
    Rundata data2 = {3, 3, (1^2^4)};
    uint8_t errado = 0xFF;
    Estado apos2 = STATE_START;
    if(step_via_protothread(errado, inicial, &data2) != apos2) {
        return "Estado não mudou de CHKSUM para START com checksum incorreto";
    }
    if(data2.chksum != 0 || data2.n != 0 || data2.qtd != 0) {
        return "Dados não foram resetados após erro de checksum";
    }
    return 0;
}

char* testar_end () {
    Estado inicial = STATE_END;
    Rundata data = {3, 3, (1^2^4)};
    uint8_t dado = 0x03;
    Estado apos = STATE_START;
    if(step_via_protothread(dado, inicial, &data) != apos) {
        return "Estado não mudou de FIM para START com ETX";
    }
    if(data.chksum != 0 || data.n != 0 || data.qtd != 0) {
        return "Dados não foram resetados após FIM";
    }
    return 0;
}

char* testar_data () {
    Estado e = STATE_DATA;
    Rundata data = {0, 3, 0};
    uint8_t dado[] = {0x01, 0x02, 0x04};
    Estado apos = STATE_CHKSUM;
    for(int i = 0; i < 3; i++) {
        e = step_via_protothread(dado[i], e, &data);
    }
    if(e != apos) {
        return "Estado não mudou de DADOS para CHKSUM";
    }
    if(data.chksum != (1^2^4)) {
        return "Erro no checksum dos dados";
    }
    return 0;
}

//Teste de integração
char* testar_frame_completo_event_driven() {
    struct protocolo_pt ctx;
    protocolo_pt_init(&ctx);
    uint8_t frame[] = {0x02, 0x03, 0x01, 0x02, 0x04, (0x01^0x02^0x04), 0x03};
    protocolo_pt_feed(&ctx, frame, sizeof(frame));
    /* precisa drenar eventos restantes: já foi feito via feed que agenda a thread */
    if (!protocolo_pt_has_frame(&ctx)) return "Frame completo não detectado";
    if (ctx.estado != STATE_START) return "Estado não voltou para START após frame";
    return 0;
}

char* testar_qtd_zero_event_driven() {
    struct protocolo_pt ctx;
    protocolo_pt_init(&ctx);
    uint8_t frame[] = {0x02, 0x00, 0x00, 0x03};
    protocolo_pt_feed(&ctx, frame, sizeof(frame));
    if (!protocolo_pt_has_frame(&ctx)) return "Frame QTD=0 não detectado";
    return 0;
}

char* testar_checksum_erro_event_driven() {
    struct protocolo_pt ctx;
    protocolo_pt_init(&ctx);
    uint8_t bad[] = {0x02, 0x01, 0xAA, 0xFF};
    protocolo_pt_feed(&ctx, bad, sizeof(bad));
    if (!protocolo_pt_has_error(&ctx)) return "Erro de checksum não sinalizado";
    if (protocolo_pt_has_frame(&ctx)) return "Frame erroneamente aceito após checksum errado";
    /* recuperação */
    uint8_t good[] = {0x02, 0x01, 0x55, 0x55, 0x03};
    protocolo_pt_feed(&ctx, good, sizeof(good));
    if (!protocolo_pt_has_frame(&ctx)) return "Recuperação após erro falhou";
    return 0;
}

struct resultado_testes executar_testes() {
    struct resultado_testes resultado = {0, 0};
    Test a_testar[] = {
        testar_start_1, testar_start_2, testar_qtd_1, testar_qtd_2,
        testar_data, testar_checksum, testar_end,
        testar_frame_completo_event_driven, testar_qtd_zero_event_driven, testar_checksum_erro_event_driven
    };
    int length = sizeof(a_testar) / sizeof(a_testar[0]);
    for (int i = 0; i < length; i++) {
        char* valor = a_testar[i]();
        resultado.testes_realizados++;
        if (valor) {
            resultado.mensagem = valor;
            break;
        }
    }
    return resultado;
}

int main() {
    printf("Testando (protothreads)...\n");
    struct resultado_testes resultado = executar_testes();
    printf("Realizados %d testes\n", resultado.testes_realizados);
    if(!resultado.mensagem) {
        printf("Testes completos!\n");
        return 0;
    } else {
        printf("%s\n", resultado.mensagem);
        return 1;
    }
}
