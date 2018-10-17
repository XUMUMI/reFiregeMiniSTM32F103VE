#include "w25x.h"

#include "stm32f10x.h"

void spiGPIOConf(void);
void spiConf(void);

void spiInit(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    spiGPIOConf();
    spiConf();
    spiSw(true);
    SPI_Cmd(SPI1, ENABLE);
}

void spiSw(bool sw)
{
    if(sw) GPIO_ResetBits(GPIOC, GPIO_Pin_0);
    else GPIO_SetBits(GPIOC, GPIO_Pin_0);
}

uint8_t spiSendByte(uint8_t byte)
{
    uint8_t ret = 0;

    while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE))continue;
    SPI_I2S_SendData(SPI1, byte);
    
    while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE))continue;
    ret = SPI_I2S_ReceiveData(SPI1);
    
    return ret;    
}

uint8_t spiSendCode(W25XCode code)
{
    return spiSendByte(code);
}

void spiSendAddr(addrtype addr)
{
    spiSendByte(*(addr++));
    spiSendByte(*(addr++));
    spiSendByte(*(addr++));
}

void spiIO(size_t oargc, uint8_t *oargv, size_t iargc, W25XCode *iargv)
{
    spiSw(true);
    while(iargc--) spiSendCode(*(iargv++));
    while(oargc--) *(oargv++) = spiSendCode(DUMMY) & FLASH_W25X_CODE_MASK;
    spiSw(false);
}

void spiHPMID(uint8_t *HPMID)
{
    W25XCode code[] = 
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

void spiMFID(uint8_t *MFID, uint8_t *HPMID)
{    
    W25XCode code[] = 
    {
        W25X_Manufacturer_Device_ID, 
        DUMMY, 
        DUMMY, 
        Dummy_Byte, 
    };
    uint8_t ret[2];
    spiIO(2, ret, 4, code);
    *MFID = ret[0];
    *HPMID = ret[1];
}

void spiUqID(uint8_t *UqID)
{
    W25XCode code[] = 
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

void spiJEDECID(uint8_t *manufacturer, uint8_t *memory_type, uint8_t *capacity)
{
    W25XCode code[] = 
    {
        W25X_JEDEC_ID, 
    };
    uint8_t ret[3];
    spiIO(3, ret, 1, code);
    *manufacturer = ret[0];
    *memory_type = ret[1];
    *capacity = ret[2];
}

void spiInSw(bool sw)
{
    W25XCode code[] = 
    {
        W25X_Write_Disable, 
    };
    if(sw) code[0] = W25X_Write_Enable;
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
    W25XCode code[] = 
    {
        W25X_Read_Data, 
        (W25XCode)addr[0],
        (W25XCode)addr[1],
        (W25XCode)addr[2],
    };
    spiIO(len, (uint8_t*)cont, 4, code);
}

void spiPowerDown(void)
{
    W25XCode code[] = {W25X_Power_down, };
    spiIO(0, NULL, 1, code);
}

void spiGPIOConf(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    GPIO_InitTypeDef gpiostr;
    gpiostr.GPIO_Mode = GPIO_Mode_AF_PP;
    gpiostr.GPIO_Speed = GPIO_Speed_50MHz;

    /*CLK*/
    gpiostr.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOA, &gpiostr);
    /*DIO*/
    gpiostr.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOA, &gpiostr);
    /*DO*/
    gpiostr.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpiostr.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOA, &gpiostr);

    /*CS*/
    gpiostr.GPIO_Mode = GPIO_Mode_Out_PP;
    gpiostr.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOC, &gpiostr);
}

void spiConf(void)
{
    SPI_InitTypeDef spistr;
    spistr.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    spistr.SPI_CPHA = SPI_CPHA_1Edge;
    spistr.SPI_CPOL = SPI_CPOL_Low;
    spistr.SPI_CRCPolynomial = 0;
    spistr.SPI_DataSize = SPI_DataSize_8b;
    spistr.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    spistr.SPI_FirstBit = SPI_FirstBit_MSB;
    spistr.SPI_Mode = SPI_Mode_Master;
    spistr.SPI_NSS = SPI_NSS_Soft;
    SPI_Init(SPI1 , &spistr);
}

#define FLASH_ADDRESS_BIT_LENGTH 24

addrtype addr24bit(uint32_t addr)
{
    size_t length = FLASH_ADDRESS_BIT_LENGTH / 8;
    addrtype ret = (addrtype)malloc(length * sizeof(uint8_t));
    for(int i = 0; i < length; i++) ret[i] = *((addrtype)(&addr) + length - i - 1);
    return ret;
}
