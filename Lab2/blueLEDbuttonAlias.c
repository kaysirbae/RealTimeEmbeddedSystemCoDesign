/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
/* Standard Includes */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    volatile uint32_t ii;

/* Halting the Watchdog */
    MAP_WDT_A_holdTimer();
    //Initializing LED's as output pins
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0); //red
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);//green
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);//blue

    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
    //setting LED's to known state low
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
    
    //assignment for blue LED alias
    uint8_t *port_addr = 0x40004C03; uint8_t bit_pos = 0x2;
    uint8_t *peripheral_region_addr = 0x40000000; uint8_t *alias_region_addr = 0x42000000;

    uint8_t *alias_addrblue = (port_addr - peripheral_region_addr) *32 + bit_pos * 4 + alias_region_addr;

    //assignment for Button Alias
    uint8_t *port_buttonaddr = 0x40004C00; uint8_t bit_buttonpos = 0x1;
    uint8_t *peripheral_region_buttonaddr = 0x40000000; uint8_t *alias_region_buttonaddr = 0x42000000;

    uint8_t *alias_buttonaddr = (port_buttonaddr - peripheral_region_buttonaddr) *32 + bit_buttonpos * 4 + alias_region_buttonaddr;

while (1)
{
/* Delay Loop*/
    for(ii=0;ii<50000;ii++){
    }
    printf("\nAddress of Blue light alias: 0x%x\n Address of button alias: 0x%x",alias_addrblue,alias_buttonaddr);
    if(*alias_buttonaddr) *alias_addrblue = 0;
    else *alias_addrblue = 1;
}
}
