#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

static uint8_t cipherKey[32] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                                0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f };
static uint8_t DataAESdecrypted[16]; //decrypted
static uint8_t DataAESencrypted[16]; //encrypted

char* message = "Love Yourself";

//function declaration
void decrypt_message_16(uint8_t* data,uint8_t* decrypted,uint8_t* key);
void encrypt_message_16(char* str,uint8_t* encrypted,uint8_t* key);

int main(void)
{
    /* Stop Watchdog */
    MAP_WDT_A_holdTimer();

    encrypt_message_16(message,DataAESencrypted,cipherKey);
    decrypt_message_16(DataAESencrypted,DataAESdecrypted,cipherKey);
    printf("\nBefore Encrypting Data \t\t After Encrypting Data \t\t After Decrypting Data \n");
    printf("\t 0x%x \t\t\t\t 0x%x \t\t\t\t 0x%x\n",*message,*DataAESencrypted,*DataAESdecrypted);
    while(1)
    {
        MAP_PCM_gotoLPM0();
    }
}
void encrypt_message_16(char* str,uint8_t* encrypted,uint8_t* key){
    /* Load a cipher key to module */
    MAP_AES256_setCipherKey(AES256_BASE, key, AES256_KEYLENGTH_256BIT); /* Encrypt data with preloaded cipher key */
    MAP_AES256_encryptData(AES256_BASE,str, encrypted);
}
void decrypt_message_16(uint8_t* data,uint8_t* decrypted,uint8_t* key){ /* Load a decipher key to module */
    MAP_AES256_setDecipherKey(AES256_BASE, key,AES256_KEYLENGTH_256BIT); /* Decrypt data with keys that were generated during encryption - takes 214 MCLK cyles. This function will generate all round keys needed for decryption first and then the encryption process starts */
    MAP_AES256_decryptData(AES256_BASE, data, decrypted);
}
