#include <stdlib.h>
#include <string.h>

#include "led.h"
#include "usart.h"
#include "w25x.h"

void initSTM(void);
void showFlashInfo(void);

int main(void)
{
    initSTM();
	
    /*Read info*/
    ledSet(Color_Red, true);
    showFlashInfo();
    ledSet(Color_Red, false);
    
    /*Erase & Write data*/
    ledSet(Color_Green, true);
    char teststr[] = "XUMUMI";
    addrtype addr = addr24bit(0x1BFA02);
    spiStErase(addr);
    spiInPage(addr, teststr, sizeof(teststr));
    ledSet(Color_Green, false);
    
    /*Read data*/
    ledSet(Color_Blue, true);
    char *buf = (char*)malloc(sizeof(teststr) * sizeof(char));
    spiReadData(buf, addr, sizeof(teststr));
    printf("%s", buf);
    ledSet(Color_Blue, false);
    
    /*Power down*/
    spiPowerDown();
}

void initSTM(void)
{
    ledInit();
    usartInit();
    spiInit();
}

void showFlashInfo(void)
{
    /*W25X_HPM_Device_ID*/
    uint8_t HPMID;
    spiHPMID(&HPMID);
    printf(
				"HPM Device ID: 0X%02X" "\r\n\r\n", 
				HPMID
		);

    /*W25X_Manufacturer_Device_ID*/
    uint8_t MFID;
    spiMFID(&HPMID, &MFID);
    printf(
				"Manufacturer Device ID: 0X%02X" "\r\n"
        "HPM Device ID: 0X%02X" "\r\n\r\n", 
        HPMID, MFID
		);

    /*W25X_Read_Unique_ID*/
    uint8_t *uqID = (uint8_t*)malloc(FLASH_W25X_UNIQUE_ID_LENGTH * sizeof(uint8_t));
    memset(uqID, 0, FLASH_W25X_UNIQUE_ID_LENGTH * sizeof(*uqID));
	
    spiUqID(uqID);
		
    printf("Unique ID: ");
    for(uint8_t i = 0; i < FLASH_W25X_UNIQUE_ID_LENGTH; ++i) printf("%02X", uqID[i]);
    printf("\r\n\r\n");
		
    free(uqID);

    /*W25X_JEDEC_ID*/
    uint8_t manfac, memtyp, capty;
    spiJEDECID(&manfac, &memtyp, &capty);
    printf(
            "Manufacturer Device ID:0X%02X" "\r\n"
            "Memory Type: 0X%02X" "\r\n"
            "Capacity: 0X%02X" "\r\n\r\n", 
            manfac, memtyp, capty
          );
}

void USART1_IRQHandler(void)
{
    if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE)) 
    {
        /*usartSendByte(USART_ReceiveData(USART1));*/
    }
}
