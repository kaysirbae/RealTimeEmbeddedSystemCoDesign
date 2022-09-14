#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

int main(void)
{
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);
    printf("%u\n", MAP_CS_getMCLK());

    /* Stop Watchdog */
    MAP_WDT_A_holdTimer();

    //Initializing Blue LED as output pin and bringing LED to known state 0
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);

    //To read input state properly using pull up resistor function
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
    MAP_Timer32_getValue(TIMER32_0_BASE);

    while (1){
        if(MAP_GPIO_getInputPinValue(GPIO_PORT_P1,GPIO_PIN1)){
            MAP_Timer32_haltTimer(TIMER32_0_BASE);//hold value of timer when not pressed
            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);// if not pressing button1 turn off
            uint32_t t0 = 0xFFFFFFFF - MAP_Timer32_getValue(TIMER32_0_BASE);
            double sec = (double) (((double) 1/3000000) * 1000 * (int) t0);
            if(sec>0){
                printf("%.2f ms\n", (double) sec);
            }
            MAP_Timer32_setCount(TIMER32_0_BASE,0xFFFFFFFF);//reset count after check

        }
        else{
            MAP_Timer32_startTimer(TIMER32_0_BASE, 0);//start and keep running timer during button push
            MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);//set on if pressed
        }
    }
}



