#include <stdio.h>
#include <stdint.h>

/* Retorna a posição (0 = LSB, 31 = MSB) do primeiro bit 1
   encontrado a partir do bit mais significativo.
   Retorna -1 se valor == 0. */
int primeiro_bit_um(uint32_t valor)
{
    if (valor == 0)
        return -1;

    for (int pos = 31; pos >= 0; pos--) {
        if (valor & ((uint32_t)1 << pos))
            return pos;
    }
    return -1; /* inalcançável */
}

int main(void)
{
    struct { uint32_t entrada; int esperado; } casos[] = {
        { 0x00000067u /* 0b001100111 */, 6 },
        { 0x00000000u, -1 },
        { 0x00000001u, 0 },
        { 0x80000000u, 31 },
        { 0xFFFFFFFFu, 31 },
        { 0x00010000u, 16 },
    };
    int n = sizeof(casos) / sizeof(casos[0]);
    int falhas = 0;

    for (int i = 0; i < n; i++) {
        int r = primeiro_bit_um(casos[i].entrada);
        int ok = (r == casos[i].esperado);
        if (!ok) falhas++;
        printf("0x%08X -> %2d (esperado %2d) %s\n",
               casos[i].entrada, r, casos[i].esperado,
               ok ? "OK" : "FALHOU");
    }
    return falhas != 0;
}
