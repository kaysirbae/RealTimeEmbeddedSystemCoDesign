#include <ti/devices/msp432p4xx/driverlib/driverlib.h> 
/* Standard Includes */ #include <stdint.h> 
#include <stdbool.h> 
#include <stdio.h> 
#define CRC32_POLY 0xEDB88320 
#define CRC32_INIT 0xFFFFFFFF 
static uint8_t Data[10240]; 
static uint32_t calculateCRC32(uint8_t * data, uint32_t length); 
static uint32_t compute_simple_checksum(uint8_t * data, uint32_t length); 
volatile uint32_t Hardware_CRC32, Software_CRC32, Manual_CRC32; 
int main(void) 
{ 
 /* Stop WDT */ 
 MAP_WDT_A_holdTimer(); 
 uint32_t i; 
 MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1,TIMER32_32BIT,  TIMER32_FREE_RUN_MODE); 
 MAP_Timer32_startTimer(TIMER32_0_BASE, 0); 
 float freq = MAP_CS_getMCLK(); 
 float time = 1 / freq * 1000; 
 //Fill the array with a series of sequential numbers, starting with 0, and  increment by 1 for the following 
 //numbers. So, the series would look like 0, 1, 2, 3, 4, ... 
 for (i = 0; i < 10240; i++) { 
 Data[i] = i; 
 } 
// Start hardware calculation 
 uint32_t t0 = MAP_Timer32_getValue(TIMER32_0_BASE); 
 MAP_CRC32_setSeed(CRC32_INIT, CRC32_MODE); 
 for (i = 0; i < 10240; i++) 
 { 
 Data[i] = i; 
 MAP_CRC32_set8BitData(Data[i], CRC32_MODE); 
 } 
 uint32_t Hardware_CRC32 = MAP_CRC32_getResultReversed(CRC32_MODE) ^ 0xFFFFFFFF;  uint32_t t1 = MAP_Timer32_getValue(TIMER32_0_BASE); 
 uint32_t d0 = (t0-t1) * time; 
 printf("\nChecksum Hardware: %u | \t Duration Hardware: %ums\n",Hardware_CRC32,  d0);



 //t3 and t2 for software 
 uint32_t t2 = MAP_Timer32_getValue(TIMER32_0_BASE); 
 Software_CRC32 = calculateCRC32((uint8_t*) Data, 10240); 
 uint32_t t3 = MAP_Timer32_getValue(TIMER32_0_BASE); 
 uint32_t d1 = (t2 - t3) * time; 
 printf("\Checksum Software: %u | \t Duration Software: %ums\n",Software_CRC32,  d1); 
 // t4 and t5 for software 
 // d2 manual calculation 
 uint32_t t4 = MAP_Timer32_getValue(TIMER32_0_BASE); 
 uint32_t Manual_CRC32 = compute_simple_checksum((uint8_t*) Data, 10240);  uint32_t t5 = MAP_Timer32_getValue(TIMER32_0_BASE); 
 uint32_t d2 = (t4 - t5) * time; 
 printf("\Checksum Simple Checksum: %u | \t Duration Simple Checksum:  %ums\n",Manual_CRC32, d2); 
 // compute the speedup of using the accelerator over 
 //the pure software method. 
 float temp1 = d0; 
 float temp2 = d1; 
 float speed = (temp1/temp2) * 100; 
 printf("\n\nSpeed Up: (Hardware Accelerator / Software Method) * 100");  printf("\nSpeed up by: %.2f ", speed); 
 __no_operation(); 
} 
// Provided from driverlib. 
static uint32_t calculateCRC32(uint8_t * data, uint32_t length) {  uint32_t i, j, byte, CRC, mask; 
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
static uint32_t compute_simple_checksum(uint8_t* data, uint32_t length) {  uint32_t result = 0x0; 
 int i; 
 for (i=0; i<length; i++) 
 { 
 result = result + data[i]; 
 } 
 return ~result; 
}
