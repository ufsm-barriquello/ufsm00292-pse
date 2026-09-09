#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "Receptor.h"


#define STX 0x02
#define ETX 0x03
#define MAX_DATA 256

typedef enum{
    ST_STX, ST_QTD, ST_DATA, ST_CHK, ST_ETX
} RX_State;

void Receptor_HandlePackage(uint8_t *rx_buffer, uint8_t rx_index)
{
    printf("Data Received successfully\n");
    printf("Data received:");
    for(int cont = 0; cont < rx_index; cont++)
    {
        printf(" %#X", rx_buffer[cont]);
    }
    printf("\n");

}

void Receptor_Run(uint8_t data)
{
    static RX_State rx_state = ST_STX;
    static uint8_t rx_buffer[MAX_DATA];
    static uint8_t rx_index = 0;
    static uint8_t rx_checksum = 0;
    static uint8_t rx_received = 0;
    static uint8_t rx_qtd = 0;
    //printf("%#X ", rx_index);
    switch(rx_state)
    {
        case ST_STX:
            if(data == STX)
            {
                rx_buffer[rx_index] = data;
                rx_state = ST_QTD;
                rx_checksum = 0;
                rx_index = 1;
            }
            break;

        case ST_QTD:
            rx_qtd = data;
            rx_buffer[rx_index] = rx_qtd;
            //printf("rxqtd %#X\n", rx_qtd);
            rx_index = 2;
            if(rx_qtd == 0)
            {
                rx_state = ST_STX; //Ignores transmission since there's no data
            }
            else
            {
                rx_checksum ^= data;
                rx_state = ST_DATA;
            }
            break;

        case ST_DATA:
            rx_buffer[rx_index] = data;
            printf("received %#X    ", rx_buffer[rx_index]);
            printf("index: %i\n ", rx_index);
            //printf("qtd: %#X\n", rx_qtd);
            rx_index++;
            rx_checksum ^= data;
            if(rx_index >= rx_qtd+2)
            {
                rx_state = ST_CHK;
            }
            break;

        case ST_CHK:
            rx_received = data;
            rx_state = ST_ETX;

        case ST_ETX:
            rx_buffer[rx_index] = data;
            if(rx_buffer[rx_index] == ETX)
            {
                //printf("a");
                rx_index++;
                Receptor_HandlePackage(rx_buffer, rx_index);
                rx_state = ST_STX;
            }
            else{
                printf("PACKAGE MISSING END OF TRANSMISSION ETX\n");
            }
            break;

        default:
            rx_state = ST_STX;
    }
}
