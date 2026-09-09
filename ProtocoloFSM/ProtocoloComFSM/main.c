#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "Receptor.h"
#include "Transmitter.h"

#define STX 0x02
#define ETX 0x03


int main()
{
    uint8_t data[] = {0xA0, 0x10, 0xC1, 0x05};
    /*uint8_t dataTransmit[qtd_data+3];
    dataTransmit[0] = STX;
    dataTransmit[1] = qtd_data;
    //printf("%i\n", qtd_data);
    for(int i = 2; i < qtd_data+3; i++)
    {
        dataTransmit[i] = data[i-2];
    }
    dataTransmit[qtd_data+2] = ETX;
    //printf("%#X", qtd_data)
    for(int cont = 0; cont < qtd_data+3; cont++)
    {
        printf("transmitted: %#X\n", dataTransmit[cont]);
        Receptor_Run(dataTransmit[cont]);
    }*/
    Transmitter(data, sizeof(data));
    uint8_t data2[] = {0x4, 0x1D};
    //qtd_data = sizeof(data2);
    Transmitter(data2, sizeof(data2));

    return 0;
}
