#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
/* Standard Includes */ #include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#define CRC32_POLY 0xEDB88320
#define CRC32_INIT 0xFFFFFFFF
static uint8_t Data[10240];
static uint32_t calculateCRC32(uint8_t * data, uint32_t length);
static uint32_t compute_simple_checksum(uint8_t * data, uint32_t length);
volatile uint32_t hardwareCRC, softwareCRC, myCRC;
int main(void)
{
    /* Stop WDT */
    MAP_WDT_A_holdTimer();
    uint32_t i;
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1,TIMER32_32BIT,  TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);
    float freq = MAP_CS_getMCLK();
    float time = 1 / freq * 1000;
    //Fill the array
    for (i = 0; i < 10240; i++) {
        Data[i] = i;
    }
    //Hardware Calculations
    uint32_t hardwaret0 = MAP_Timer32_getValue(TIMER32_0_BASE);
    MAP_CRC32_setSeed(CRC32_INIT, CRC32_MODE);
    for (i = 0; i < 10240; i++)
    {
        Data[i] = i;
        MAP_CRC32_set8BitData(Data[i], CRC32_MODE);
    }
    uint32_t hardwareCRC = MAP_CRC32_getResultReversed(CRC32_MODE) ^ 0xFFFFFFFF;  uint32_t hardwaret1 = MAP_Timer32_getValue(TIMER32_0_BASE);
    uint32_t hardwareDeltaT = (hardwaret0 - hardwaret1) * time;
    printf("\nChecksum Hardware: %u | \t Duration Hardware: %ums\n",hardwareCRC,  hardwareDeltaT);

    //Software Calculations
    uint32_t softwaret0 = MAP_Timer32_getValue(TIMER32_0_BASE);
    softwareCRC = calculateCRC32((uint8_t*) Data, 10240);
    uint32_t softwaret1 = MAP_Timer32_getValue(TIMER32_0_BASE);
    uint32_t softwareDeltaT = (softwaret0 - softwaret1) * time;
    printf("\Checksum Software: %u |  Duration Software: %ums\n",softwareCRC,  softwareDeltaT);

    // Manual Checksum Calculations
    uint32_t myt0 = MAP_Timer32_getValue(TIMER32_0_BASE);
    uint32_t myCRC = compute_simple_checksum((uint8_t*) Data, 10240);
    uint32_t myt1 = MAP_Timer32_getValue(TIMER32_0_BASE);
    uint32_t myDeltaT = (myt0 - myt1) * time;
    printf("\Checksum Simple Checksum: %u |  Duration Simple Checksum:  %ums\n",myCRC, myDeltaT);

    //Speedup Calculation
    float temp1 = hardwareDeltaT;
    float temp2 = softwareDeltaT;
    float speed = (temp1/temp2) * 100;
    printf("\n\nSpeed Up: (Hardware Accelerator / Software Method) * 100");  printf("\nSpeed up by: %.2f ", speed);
    __no_operation();
}

static uint32_t calculateCRC32(uint8_t * data, uint32_t length) {
    uint32_t i, j, byte, CRC, mask;
    CRC = 0xFFFFFFFF;
    for (i = 0; i < length; i++) {
        byte = data[i];
        CRC = CRC ^ byte;
        for (j = 0; j < 8; j++) {
            mask = -(CRC & 1);
            CRC = (CRC >> 1) ^ (CRC32_POLY & mask);
        }
    }
    return ~CRC;
}
static uint32_t compute_simple_checksum(uint8_t* data, uint32_t length) {
    uint32_t ans = 0x0;
    int i;
    for (i=0; i<length; i++)
    {
        ans = ans + data[i];
    }
    return ~ans;
}

