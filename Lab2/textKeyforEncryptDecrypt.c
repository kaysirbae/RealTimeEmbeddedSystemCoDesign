#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

static uint8_t cipherKey[32] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                                0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f };
static uint8_t DataAESdecrypted[16]; //decrypted
static uint8_t DataAESencrypted[16]; //encrypted
static uint8_t Data[1600] = {};
uint8_t len; //data length

//function declaration
void printHex(uint8_t arr[]);
void printString(uint8_t arr[]);
void decrypt_message_16(uint8_t* data,int data_length, uint8_t* decrypted, uint8_t* key);
void encrypt_message_16(char* str, uint8_t* encrypted, uint8_t* key);
char* message = "This Lab Fun!";

int main(void)
{
    char key1[] = "Password is CMPE146-03";
    char key2[] = "Password is CMPE146-03";

    /* Stop Watchdog */
    MAP_WDT_A_holdTimer();

    encrypt_message_16(message, DataAESencrypted, key1);
    decrypt_message_16(DataAESencrypted, len, DataAESdecrypted, key2);

    printf("\nBefore encrypt data\n");
    printf("Hex: ");
    printHex(Data);
    printf("\nString: ");
    printString(Data);
    printf("\nAfter encrypt data\n");
    printf("Hex: ");
    printHex(DataAESencrypted);
    printf("\nAfter decrypt data\n ");
    printf("Hex: ");
    printHex(DataAESdecrypted);
    printf("\nString: ");
    printString(DataAESdecrypted);
    printf("\n");


    while(1)
    {
        MAP_PCM_gotoLPM0();
    }
}
void printHex(uint8_t arr[]){
    int i;
    for(i = 0; i < len; i++){
        printf("0x%x\t", arr[i]);
    }
}

void printString(uint8_t arr[]){
    printf("%s", arr);
}

void encrypt_message_16(char* str,uint8_t* encrypted,uint8_t* key){
    int index = 0;
    uint8_t* savedData = Data;
    uint8_t* encryptingThis = encrypted;

    //must read character untill end of string
    while(str[index] != '\0'){
        Data[index] = str[index];
        index++;
    }
    Data[index] = '\0';
    len = index;

    /* Load a cipher key to module */
    MAP_AES256_setCipherKey(AES256_BASE, key, AES256_KEYLENGTH_256BIT); /* Encrypt data with preloaded cipher key */
    for(index = 0; index < (len/16) + 1; index++){
        MAP_AES256_encryptData(AES256_BASE, savedData, encryptingThis);
        savedData += 16; //increment
        encryptingThis += 16;
    }

}
void decrypt_message_16(uint8_t* data,int data_length, uint8_t* decrypted,uint8_t* key){ /* Load a decipher key to module */
    int i;
    uint8_t* decryptingThis = data;
    uint8_t* unecryptedValue = decrypted;

    MAP_AES256_setDecipherKey(AES256_BASE, key,AES256_KEYLENGTH_256BIT); /* Decrypt data with keys that were generated during encryption - takes 214 MCLK cyles. This function will generate all round keys needed for decryption first and then the encryption process starts */
    for(i = 0; i < (len/16) + 1; i++){
        MAP_AES256_decryptData(AES256_BASE, decryptingThis, unecryptedValue);
        decryptingThis += 16;
        unecryptedValue += 16;
    }
}
