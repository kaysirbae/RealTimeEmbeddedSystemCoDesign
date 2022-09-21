#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#define CRC32_POLY  0xEDB88320
#define CRC32_INIT  0xFFFFFFFF

static uint8_t Data[10240] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};
static uint32_t calculateCRC32(uint8_t* data, uint32_t length);
static void corruptor(uint8_t* data, int pos);
static uint32_t compute_simple_checksum(uint8_t* data, uint32_t length);

volatile uint32_t hardwareCRC, softwareCRC;

int main(void)
{
    /* Stop WDT */
    MAP_WDT_A_holdTimer();

    uint32_t i;

    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);

    float freq = MAP_CS_getMCLK();
    float time = 1/freq * 1000;

    uint32_t hardware_t0 = MAP_Timer32_getValue(TIMER32_0_BASE);

    MAP_CRC32_setSeed(CRC32_INIT, CRC32_MODE);

    for(i = 0; i < 10240; i++){
        Data[i] = i;
        MAP_CRC32_set8BitData(Data[i], CRC32_MODE);
    }

    hardwareCRC = MAP_CRC32_getResultReversed(CRC32_MODE) ^ 0xFFFFFFFF;
    uint32_t hardware_t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
    uint32_t hardware_deltaT = (hardware_t0 - hardware_t1) * time;

    uint32_t software_t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
    softwareCRC = calculateCRC32((uint8_t*) Data, 10240);
    uint32_t software_t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
    uint32_t software_deltaT = (software_t0 - software_t1) * time;

    uint32_t t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
    uint32_t calculated_CRC = compute_simple_checksum((uint8_t*) Data, 10240);
    uint32_t t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
    uint32_t deltaT = (t0 - t1) * time;

    float speedTime = hardware_deltaT / software_deltaT;

    printf("\n CRC: %u\n", calculated_CRC);
    printf("CRC Time: %ums\n", deltaT);
    printf("Hardware CRC: %u\n", hardwareCRC);
    printf("Hardware Time: %ums\n", hardware_deltaT);
    printf("Software CRC: %u\n", softwareCRC);
    printf("Software Time: %ums\n", software_deltaT);
    printf("Speed Up Time: %u\n", speedTime);

    corruptor(Data, 21);

    hardware_t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
    hardwareCRC = MAP_CRC32_getResultReversed(CRC32_MODE) ^ 0xFFFFFFFF;
    hardware_t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
    hardware_deltaT = (hardware_t0 - hardware_t1) * time;

    software_t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
    softwareCRC = calculateCRC32((uint8_t*) Data, 10240);
    software_t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
    software_deltaT = (software_t0 - software_t1) * time;

    t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
    calculated_CRC = compute_simple_checksum((uint8_t*) Data, 10240);
    t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
    deltaT = (t0 - t1) * time;
    speedTime = hardware_deltaT / software_deltaT;

    printf("\n CRC: %u\n", calculated_CRC);
    printf("CRC Time: %ums\n", deltaT);
    printf("Hardware CRC: %u\n", hardwareCRC);
    printf("Hardware Time: %ums\n", hardware_deltaT);
    printf("Software CRC: %u\n", softwareCRC);
    printf("Software Time: %ums\n", software_deltaT);
    printf("Speed Up Time: %u\n", speedTime);

    corruptor(Data, 20);

    hardware_t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
    hardwareCRC = MAP_CRC32_getResultReversed(CRC32_MODE) ^ 0xFFFFFFFF;
    hardware_t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
    hardware_deltaT = (hardware_t0 - hardware_t1) * time;

    software_t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
    softwareCRC = calculateCRC32((uint8_t*) Data, 10240);
    software_t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
    software_deltaT = (software_t0 - software_t1) * time;

    t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
    calculated_CRC = compute_simple_checksum((uint8_t*) Data, 10240);
    t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
    deltaT = (t0 - t1) * time;
    speedTime = hardware_deltaT / software_deltaT;

    printf("\n CRC: %u\n", calculated_CRC);
    printf("CRC Time: %ums\n", deltaT);
    printf("Hardware CRC: %u\n", hardwareCRC);
    printf("Hardware Time: %ums\n", hardware_deltaT);
    printf("Software CRC: %u\n", softwareCRC);
    printf("Software Time: %ums\n", software_deltaT);
    printf("Speed Up Time: %u\n", speedTime);

    __no_operation();
}

void corruptor(uint8_t* data, int pos){
    printf("data[position]: %i position: %i\n", data[pos], pos);
    data[pos] ^= 1;
    printf("data[position]: %i position: %i\n", data[pos], pos);
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
