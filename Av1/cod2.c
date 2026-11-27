#include <stdio.h>
#include <stdint.h>

#define BCD_INVALIDO 0xFFFFu

/* Converte um decimal de 0 a 9999 para BCD empacotado (16 bits).
   Retorna BCD_INVALIDO se valor > 9999. */
uint16_t decimal_para_bcd(uint16_t valor)
{
    if (valor > 9999)
        return BCD_INVALIDO;

    uint16_t bcd = 0;

    for (int nibble = 0; nibble < 4; nibble++) {
        uint16_t digito = valor % 10;      /* dígito menos significativo */
        bcd |= (uint16_t)(digito << (nibble * 4));
        valor /= 10;
    }
    return bcd;
}

static void imprime_binario16(uint16_t v)
{
    for (int i = 15; i >= 0; i--) {
        putchar((v >> i) & 1 ? '1' : '0');
        if (i % 4 == 0 && i != 0)
            putchar(' ');
    }
}

int main(void)
{
    struct { uint16_t entrada; uint16_t esperado; } casos[] = {
        { 0,     0x0000 },
        { 5,     0x0005 },
        { 9,     0x0009 },
        { 10,    0x0010 },
        { 99,    0x0099 },
        { 100,   0x0100 },
        { 1234,  0x1234 },
        { 9999,  0x9999 },
        { 10000, BCD_INVALIDO },
        { 65535, BCD_INVALIDO },
    };
    int n = sizeof(casos) / sizeof(casos[0]);
    int falhas = 0;

    for (int i = 0; i < n; i++) {
        uint16_t r = decimal_para_bcd(casos[i].entrada);
        int ok = (r == casos[i].esperado);
        if (!ok) falhas++;

        printf("%5u -> 0x%04X [", casos[i].entrada, r);
        imprime_binario16(r);
        printf("] (esperado 0x%04X) %s\n", casos[i].esperado,
               ok ? "OK" : "FALHOU");
    }

    printf("\n%s\n", falhas == 0 ? "Todos os testes passaram."
                                 : "Houve falhas.");
    return falhas != 0;
}
