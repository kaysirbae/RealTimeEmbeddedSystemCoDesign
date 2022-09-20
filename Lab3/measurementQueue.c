#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#define QUEUE_SIZE 10

struct record queue[QUEUE_SIZE];
volatile int read_index;
volatile int write_index;

struct record{
    uint32_t timestamp;
    uint8_t state;
};

int main(void)
{
    float Button_Duration;
    float frequency = MAP_CS_getMCLK();
    float time = 1/frequency * 1000;

    write_index = 0;
    read_index = 0;


    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);
    printf("%u\n", MAP_CS_getMCLK());

    //Set Red Green and Blue as Output Pin
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
    //Set all LED's at known state low
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);

    /* Stop Watchdog */
    MAP_WDT_A_holdTimer();

    //To read input state properly using pull up resistor function
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
    //Setting Up Interrupt
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);
    MAP_Interrupt_enableInterrupt(INT_PORT1);

    MAP_Interrupt_enableMaster();

    MAP_GPIO_interruptEdgeSelect(GPIO_PORT_P1, GPIO_PIN1, GPIO_HIGH_TO_LOW_TRANSITION);

    while (1){
        _delay_cycles(200 * 3000);
        if(read_index != write_index){
            read_index = (read_index + 1) % QUEUE_SIZE;
            if(queue[read_index].state == 1){
                printf("Current Index: %d Next Index: %d \n", (read_index), read_index + 1);
                printf("Current State: %d Next State: %d\n", queue[read_index].state, queue[read_index + 1].state);

                Button_Duration = (time * (queue[(read_index - 1)].timestamp - queue[read_index].timestamp));
                printf("Button Duration: %f ms\n", Button_Duration);
            }
        }
    }
}

void PORT1_IRQHandler(void){
    uint32_t status;

    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, status);

    uint8_t state = MAP_GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1);

    if(status & GPIO_PIN1){
        if(state == 0){
            MAP_GPIO_interruptEdgeSelect(GPIO_PORT_P1, GPIO_PIN1, GPIO_LOW_TO_HIGH_TRANSITION);
            MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);

            write_index = (write_index + 1) % QUEUE_SIZE;
            queue[write_index].state = state;
            uint32_t s0 = MAP_Timer32_getValue(TIMER32_0_BASE);
            queue[write_index].timestamp = s0;
        }
        if(state == 1){
            MAP_GPIO_interruptEdgeSelect(GPIO_PORT_P1, GPIO_PIN1, GPIO_HIGH_TO_LOW_TRANSITION);
            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
            write_index = (write_index + 1) % QUEUE_SIZE;
            queue[write_index].state = state;
            uint32_t s1 = MAP_Timer32_getValue(TIMER32_0_BASE);
            queue[write_index].timestamp = s1;
        }
    }
}
