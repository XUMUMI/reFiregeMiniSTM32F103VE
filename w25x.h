#ifndef __W25X_H
#define __W25X_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define FLASH_W25X_UNIQUE_ID_LENGTH 8
#define FLASH_W25X_PAGE_SIZE 256 - 1
#define FLASH_W25X_CODE_MASK 0xFF
#define FLASH_W25X_WRITE_STATUS_MASK 0x01

typedef uint8_t* addrtype;

typedef enum
{
    W25X_Write_Enable               = 0x06,
    W25X_Write_Disable              = 0x04,
    W25X_Read_Status_Register1      = 0x05,
    W25X_Read_Status_Register2      = 0x35,
    W25X_Write_Status_Register      = 0x01,
    W25X_Page_Program               = 0x02,
    W25X_Quad_Page_Program          = 0x32,
    W25X_Block_Erase_64KB           = 0xD8,
    W25X_Block_Erase_32KB           = 0x52,
    W25X_Sector_Erase_4KB           = 0x20,
    W25X_Chip_Erase                 = 0xC7,
    W25X_Erase_Suspend              = 0x75,
    W25X_Erase_Resume               = 0x7A,
    W25X_Power_down                 = 0xB9,
    W25X_High_Performance_Mode      = 0xA3,
    W25X_Continuous_Read_Mode_Reset = 0xFF,
    W25X_HPM_Device_ID              = 0xAB,
    W25X_Manufacturer_Device_ID     = 0x90,
    W25X_Read_Unique_ID             = 0x4B,
    W25X_JEDEC_ID                   = 0x9F,
    
    W25X_Read_Data                  = 0x03,
    W25X_Fast_Read                  = 0x0B,
    W25X_Fast_Read_Dual_Output      = 0x3B,
    W25X_Fast_Read_Dual_IO          = 0xBB,
    W25X_Fast_Read_Quad_Output      = 0x6B,
    W25X_Fast_Read_Quad_IO          = 0xEB,
    Octal_Word_Read_Quad_IO         = 0xE3,

    DUMMY                           = 0xA1,
    Dummy_Byte                      = 0x00
}W25XCode;

void spiInit(void);
void spiSw(bool sw);
uint8_t spiSendByte(uint8_t byte);
uint8_t spiSendCode(W25XCode code);
void spiSendAddr(addrtype addr);
void spiIO(size_t oargc, uint8_t *oargv, size_t iargc, W25XCode *iargv);
void spiPowerDown(void);

void spiHPMID(uint8_t *HPMID);
void spiMFID(uint8_t *MFID, uint8_t *HPMID);
void spiUqID(uint8_t *UqID);
void spiJEDECID(uint8_t *manufacturer, uint8_t *memory_type, uint8_t *capacity);

void spiInWait(void);
void spiInSw(bool sw);
void spiStErase(uint8_t *addr);
void spiInPage(addrtype addr, char *cont, size_t len);
void spiReadData(char *cont, addrtype addr, size_t len);

uint8_t* addr24bit(uint32_t addr);

#endif
