/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include<stdio.h>
#include <string.h>
#include <stdbool.h>
#define CRC32_SEED 0xFFFFFFFF
/* Statics */
static volatile uint32_t crcSignature;

/* DMA Control Table */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_ALIGN(controlTable, 1024)
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment=1024
#elif defined(__GNUC__)
__attribute__ ((aligned (1024)))
#elif defined(__CC_ARM)
__align(1024)
#endif
uint8_t controlTable[1024];

/* Extern */
extern uint8_t data_array[1024];
int size_array[] = {2, 4, 16, 32, 64, 128, 256, 786, 1024};
volatile int dma_done = 0;

int main(void)
{
    /* Halting Watchdog */
    MAP_WDT_A_holdTimer();


    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);

    float freq = MAP_CS_getMCLK();
    float time = 1/freq * 1000;

    //DMA outside of loop
    MAP_DMA_enableModule();
    MAP_DMA_setControlBase(controlTable);

    int i;

    for(i = 0; i < 9; i++){
        int block_size = size_array[i];
        printf("Block Size: %i\n", block_size);

        uint32_t hardware_t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
        MAP_CRC32_setSeed(CRC32_SEED, CRC32_MODE);
        int ii;

        for(ii = 0; ii < block_size; ii++){
            MAP_CRC32_set8BitData(data_array[ii], CRC32_MODE);
        }
        uint32_t hardwareCRC = MAP_CRC32_getResult(CRC32_MODE);

        uint32_t hardware_t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
        uint32_t hardware_deltaT = (hardware_t0 - hardware_t1) * time;

        printf("\nHardware CRC = %08x\n", hardwareCRC);
        printf("Hardware CRC Time = %u ms\n", hardware_deltaT);

        uint32_t dma_time0 = MAP_Timer32_getValue(TIMER32_0_BASE);
        dma_done = 0;

        /* Configuring DMA module */
        MAP_DMA_enableModule();
        MAP_DMA_setControlBase(controlTable);

        /* Setting Control Indexes. In this case we will set the source of the
         * DMA transfer to our random data array and the destination to the
         * CRC32 data in register address*/
        MAP_DMA_setChannelControl(UDMA_PRI_SELECT,
                UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_1024);
        MAP_DMA_setChannelTransfer(UDMA_PRI_SELECT,
                UDMA_MODE_AUTO, data_array,
                (void*) (&CRC32->DI32), 1024);

        /* Assigning/Enabling Interrupts */
        MAP_DMA_assignInterrupt(DMA_INT1, 0);

        MAP_Interrupt_enableInterrupt(INT_DMA_INT1);

        MAP_Interrupt_enableMaster();

        /* Enabling DMA Channel 0 */
        MAP_DMA_enableChannel(0);

        MAP_CRC32_setSeed(CRC32_SEED, CRC32_MODE);
        /* Forcing a software transfer on DMA Channel 0 */
        MAP_DMA_requestSoftwareTransfer(0);

        while(1)
        {
            if(dma_done == 1){
                break;
            }
        }
        uint32_t dma_time1 = MAP_Timer32_getValue(TIMER32_0_BASE);
        uint32_t dma_deltaT = (dma_time0 - dma_time1) * time;
        printf("dma CRC Time = %u ms\n", dma_deltaT);

        uint32_t speed = hardware_deltaT / dma_deltaT;
        printf("\nSpeed Up: DMA / Hardware = %ux\n", speed);
    }
}

/* Completion interrupt for DMA */
void DMA_INT1_IRQHandler(void)
{
    MAP_DMA_disableChannel(0);
    crcSignature = MAP_CRC32_getResult(CRC32_MODE);
    printf("dma CRC = %08x\n", crcSignature);
    dma_done = 1;
}
