#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#define CRC32_POLY  0xEDB88320
#define CRC32_INIT  0xFFFFFFFF

static const uint8_t Data[9] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};
static uint32_t calculateCRC32(uint8_t* data, uint32_t length);
static uint32_t compute_simple_checksum(uint8_t* data, uint32_t length);

volatile uint32_t hardwareCRC, softwareCRC;

int main(void)
{
    /* Stop WDT */
    MAP_WDT_A_holdTimer();

    uint32_t i;

    MAP_CRC32_setSeed(CRC32_INIT, CRC32_MODE);

    for(i = 0; i < 9; i++){
        MAP_CRC32_set8BitData(Data[i], CRC32_MODE);
    }

    hardwareCRC = MAP_CRC32_getResultReversed(CRC32_MODE) ^ 0xFFFFFFFF;
    softwareCRC = calculateCRC32((uint8_t*) Data, 9);

    uint32_t calculatedCRC = compute_simple_checksum((uint8_t*) Data, 9);

    printf("\nLab CRC: %u\n", calculatedCRC);
    printf("HardwareCRC: %u\n", hardwareCRC);
    printf("Software CRC: %u\n", softwareCRC);
    __no_operation();
}

static uint32_t calculateCRC32(uint8_t* data, uint32_t length){
    uint32_t i, j, byte, CRC, mask;
    CRC = 0xFFFFFFFF;

    for (i = 0; i < length; i++){
        byte = data[i];
        CRC = CRC ^ byte;
        for(j= 0; j < 8; j++){
            mask = -(CRC & 1);
            CRC = (CRC >> 1) ^ (CRC32_POLY & mask);
        }
    }
    return ~CRC;
}

uint32_t compute_simple_checksum(uint8_t* data, uint32_t length){
    uint32_t CRC = 0x0;
    int index = 0;
    for (index = 0; index < length; index++){
        CRC += data[index];
    }
    return ~CRC;
}
