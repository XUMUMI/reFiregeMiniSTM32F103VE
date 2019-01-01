#include "w25x.h"

#include "stm32f10x.h"

#define FLASH_W25X_UNIQUE_ID_LENGTH 8
#define FLASH_W25X_PAGE_SIZE 256 - 1
#define FLASH_W25X_CODE_MASK 0xFF
#define FLASH_W25X_WRITE_STATUS_MASK 0x01

typedef enum
{
    /* W25X Flash Write Code */
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
    W25X_JEDEC_ID        
    = 0x9F,
    /* W25X Flash Read Code */
    W25X_Read_Data                  = 0x03,
    W25X_Fast_Read                  = 0x0B,
    W25X_Fast_Read_Dual_Output      = 0x3B,
    W25X_Fast_Read_Dual_IO          = 0xBB,
    W25X_Fast_Read_Quad_Output      = 0x6B,
    W25X_Fast_Read_Quad_IO          = 0xEB,
    Octal_Word_Read_Quad_IO         = 0xE3,
    
    /* W25X Flash DUMMY Code */
    DUMMY                           = 0xFF,
    Dummy_Byte                      = 0x00
}W25X_Cmd;

/* Configuration function */
void spiGPIO_Conf   (void);
void spiConf        (void);

/* Basic function */
void spiInit        (void);
void spiSw          (bool sw);
uint8_t spiSendByte (uint8_t byte);
uint8_t spiSendCode (W25X_Cmd code);
void spiSendAddr    (addrtype addr);
void spiIO          (size_t oargc, uint8_t *oargv, size_t iargc, W25X_Cmd *iargv);
void spiPowerDown   (void);
uint8_t* addr24bit  (uint32_t addr);

/* Read information function */
void spiHPM_ID      (uint8_t *HPMID);
void spiMF_ID       (uint8_t *MFID, uint8_t *HPMID);
void spiUqID        (uint8_t *UqID);
void spiJEDEC_ID    (uint8_t *manufacturer, uint8_t *memory_type, uint8_t *capacity);

void spiInit(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    spiGPIO_Conf();
    spiConf();
    spiSw(true);
    SPI_Cmd(SPI1, ENABLE);
}

void spiSw(bool sw)
{
    if(sw)  GPIO_ResetBits(GPIOC, GPIO_Pin_0);
    else    GPIO_SetBits(GPIOC, GPIO_Pin_0);
}

uint8_t spiSendByte(uint8_t byte)
{
    uint8_t ret = 0;

    while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)) continue;
    SPI_I2S_SendData(SPI1, byte);
    
    while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE))continue;
    ret = SPI_I2S_ReceiveData(SPI1);
    
    return ret;    
}

uint8_t spiSendCode(W25X_Cmd code)
{
    return spiSendByte(code);
}

void spiSendAddr(addrtype addr)
{
    spiSendByte(*(addr++));
    spiSendByte(*(addr++));
    spiSendByte(*(addr++));
}

void spiIO(size_t oargc, uint8_t *oargv, size_t iargc, W25X_Cmd *iargv)
{
    spiSw(true);
    while(iargc--) spiSendCode(*(iargv++));
    while(oargc--) *(oargv++) = spiSendCode(DUMMY) & FLASH_W25X_CODE_MASK;
    spiSw(false);
}

void spiHPM_ID(uint8_t *HPMID)
{
    W25X_Cmd code[] = 
    {
        W25X_HPM_Device_ID, 
        DUMMY, 
        DUMMY, 
        DUMMY, 
    };
    uint8_t ret[1];
    spiIO(1, ret, 4, code);
    *HPMID = ret[0];
}

void spiMF_ID(uint8_t *MFID, uint8_t *HPMID)
{    
    W25X_Cmd code[] = 
    {
        W25X_Manufacturer_Device_ID, 
        DUMMY, 
        DUMMY, 
        Dummy_Byte, 
    };
    uint8_t ret[2];
    spiIO(2, ret, 4, code);
    *MFID  = ret[0];
    *HPMID = ret[1];
}

void spiUqID(uint8_t *UqID)
{
    W25X_Cmd code[] = 
    {
        W25X_Read_Unique_ID, 
        DUMMY, 
        DUMMY, 
        DUMMY, 
        DUMMY, 
    };
    uint8_t ret[FLASH_W25X_UNIQUE_ID_LENGTH];
    spiIO(FLASH_W25X_UNIQUE_ID_LENGTH, ret, 5, code);
    for(int i = 0; i < FLASH_W25X_UNIQUE_ID_LENGTH; i++) UqID[i] = ret[i];
}

void spiJEDEC_ID(uint8_t *manufacturer, uint8_t *memory_type, uint8_t *capacity)
{
    W25X_Cmd code[] = {W25X_JEDEC_ID, };
    uint8_t ret[3];
    spiIO(3, ret, 1, code);
    *manufacturer = ret[0];
    *memory_type  = ret[1];
    *capacity     = ret[2];
}

void spiInSw(bool sw)
{
    W25X_Cmd code[] = {W25X_Write_Disable, };
    if(sw)   code[0] = W25X_Write_Enable;
    spiIO(0, NULL, 1, code);
}

void spiInWait(void)
{
    spiSw(true);
    
    spiSendCode(W25X_Read_Status_Register1);
    uint8_t status = spiSendCode(DUMMY);
    while(status & FLASH_W25X_WRITE_STATUS_MASK) status = spiSendByte(DUMMY);
    
    spiSw(false);
}

void spiStErase(addrtype addr)
{
    spiInSw(true);
    spiInWait();
    spiSw(true);
    
    spiSendCode(W25X_Sector_Erase_4KB);
    spiSendAddr(addr);
    
    spiSw(false);
    spiInWait();
    spiInSw(false);
}

void spiInPage(addrtype addr, char *cont, size_t len)
{
    if(len > FLASH_W25X_PAGE_SIZE) len = FLASH_W25X_PAGE_SIZE;
    
    spiInSw(true);
    spiSw(true);
    
    spiSendCode(W25X_Page_Program);
    spiSendAddr(addr);
    while(len--) spiSendByte(*(cont++));
    spiSendCode(Dummy_Byte);
    
    spiSw(false);
    spiInWait();
    spiInSw(false);
}

void spiReadData(char *cont, addrtype addr, size_t len)
{
    W25X_Cmd code[] = 
    {
        W25X_Read_Data, 
        (W25X_Cmd)addr[0],
        (W25X_Cmd)addr[1],
        (W25X_Cmd)addr[2],
    };
    spiIO(len, (uint8_t*)cont, 4, code);
}

void spiPowerDown(void)
{
    W25X_Cmd code[] = {W25X_Power_down, };
    spiIO(0, NULL, 1, code);
}

void spiGPIO_Conf(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    GPIO_InitTypeDef gpiostr;
    gpiostr.GPIO_Mode = GPIO_Mode_AF_PP;
    gpiostr.GPIO_Speed = GPIO_Speed_50MHz;

    /* CLK */
    gpiostr.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOA, &gpiostr);
    /* DIO */
    gpiostr.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOA, &gpiostr);
    /* DO */
    gpiostr.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpiostr.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOA, &gpiostr);

    /* CS */
    gpiostr.GPIO_Mode = GPIO_Mode_Out_PP;
    gpiostr.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOC, &gpiostr);
}

void spiConf(void)
{
    SPI_InitTypeDef spistr;
    spistr.SPI_BaudRatePrescaler    = SPI_BaudRatePrescaler_2;
    spistr.SPI_CPHA                 = SPI_CPHA_1Edge;
    spistr.SPI_CPOL                 = SPI_CPOL_Low;
    spistr.SPI_CRCPolynomial        = 0;
    spistr.SPI_DataSize             = SPI_DataSize_8b;
    spistr.SPI_Direction            = SPI_Direction_2Lines_FullDuplex;
    spistr.SPI_FirstBit             = SPI_FirstBit_MSB;
    spistr.SPI_Mode                 = SPI_Mode_Master;
    spistr.SPI_NSS                  = SPI_NSS_Soft;
    SPI_Init(SPI1, &spistr);
}

#define FLASH_ADDRESS_BIT_LENGTH 24
addrtype addr24bit(uint32_t addr)
{
    size_t   length  = FLASH_ADDRESS_BIT_LENGTH / 8;
    addrtype ret     = (addrtype)malloc(length * sizeof(uint8_t));
    for(int i = 0; i < length; i++) ret[i] = *((addrtype)(&addr) + length - i - 1);
    return ret;
}
#undef FLASH_ADDRESS_BIT_LENGTH
