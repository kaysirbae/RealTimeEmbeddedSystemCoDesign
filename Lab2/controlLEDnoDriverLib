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

    //Initializing LED's as output pin and bringing LED to known state 0
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0); //RED
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1); //GREEN
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2); //BLUE
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0); //LOW RED
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1); //LOW GREEN
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2); //LOW BLUE

    //To read input state properly using pull up resistor function
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);

    //initialize variable for switch and port
    uint8_t *port1input = 0x40004C00;
    uint8_t *port2output = 0x40004C03;

    while(1){
        if(*port1input & 0x2){
            *port2output &= 0x00;  //resets pin of blue LED keeping everything else the same turning off blue LED when not pushed
        }else{
            *port2output |= 0x04; //LED litghts up when pushed
        }
    }
}
