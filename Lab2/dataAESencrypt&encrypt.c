/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
/* Standard Includes */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static uint8_t DataAESdecrypted[16]; //decrypted data
static uint8_t DataAESencrypted[16]; //encrypted data
static uint8_t cipherKey[32] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a,
                                  0x1b, 0x1c, 0x1d, 0x1e, 0x1f};
static uint8_t data[16] = {0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};


int main(void)
{
    /* Stop Watchdog */
    MAP_WDT_A_holdTimer();

    uint8_t index;

    //Exercise 2.1
    MAP_AES256_setCipherKey(AES256_BASE, cipherKey, AES256_KEYLENGTH_256BIT); //This will load a cipher key to the module
    MAP_AES256_encryptData(AES256_BASE, data, DataAESencrypted); //Encrypt data with the loaded cipher key

    MAP_AES256_setDecipherKey(AES256_BASE, cipherKey, AES256_KEYLENGTH_256BIT); //This will load a decipher key to the module
    //When we Decrypt data with keys that were generated during the encryption process it takes 214 MCLK cycles
    //This function generates all round keys needed for decryption first and then the encryption process starts
    MAP_AES256_decryptData(AES256_BASE, DataAESencrypted, DataAESdecrypted);
    //printing Simple AES256 Example
    printf("\nBefore Encrypting Data \t\t After Encrypting Data \t\t After Decrypting Data \n");
    for(index = 0; index < 16; index++){
        printf("\t 0x%x \t\t\t\t 0x%x \t\t\t\t 0x%x\n", data[index], DataAESencrypted[index], DataAESdecrypted[index]);
    } //after this the array dataAESdecrypted will contain the same data as array data
    while(1){
        MAP_PCM_gotoLPM0();
    }
}
