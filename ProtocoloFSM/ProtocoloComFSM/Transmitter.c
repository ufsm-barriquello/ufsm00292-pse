#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "Receptor.h"

#define STX 0x02
#define ETX 0x03
#define MAX_DATA 256



void Transmitter(uint8_t *buffer, uint8_t buffer_size)
{
    uint8_t dataTransmit[buffer_size+3];
    dataTransmit[0] = STX;
    dataTransmit[1] = buffer_size;
    for(int i = 2; i < buffer_size+3; i++)
    {
        dataTransmit[i] = buffer[i-2];
    }
    dataTransmit[buffer_size+2] = ETX;
    //printf("%#X", qtd_data)
    for(int cont = 0; cont < buffer_size+3; cont++)
    {
        printf("transmitted: %#X\n", dataTransmit[cont]);
        Receptor_Run(dataTransmit[cont]);
    }
}
