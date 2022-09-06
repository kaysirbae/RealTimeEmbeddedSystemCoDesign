/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
/* Standard Includes */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    /* Stop Watchdog */
    MAP_WDT_A_holdTimer();

    //Initializing Blue LED as output pin and bringing LED to known state 0
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);

    //To read input state properly using pull up resistor function
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);

    while(1){
        if(MAP_GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1) == 0){
            MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);
        }
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
    }
}
