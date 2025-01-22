#include "drv_flash.h"
#include "stm32f1xx_hal.h"
// 内部 Flash

static FLASH_EraseInitTypeDef EraseInitStruct;

int Drv_Flash_Erase(unsigned int PageAddr, unsigned short count)
{
	unsigned int PAGEError = 0;
	HAL_FLASH_Unlock();	
	//EraseInitStruct.Banks = ;
	EraseInitStruct.NbPages = count;
	EraseInitStruct.PageAddress = PageAddr;
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK){
		HAL_FLASH_Lock();
        return PAGEError;
	}
	// 等待FLASH总线空闲
    if(FLASH_WaitForLastOperation(HAL_MAX_DELAY) != HAL_OK)
    {
        HAL_FLASH_Lock();
        return -1;
    }
    HAL_FLASH_Lock();
	return 0;
}

int Drv_Flash_Write(unsigned int PageAddr, unsigned char *pData, int Length)
{
	HAL_FLASH_Unlock();
	unsigned int *data = (unsigned int *)pData;
	while(Length > 0)
	{
		if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, PageAddr, *pData) == HAL_OK){
			PageAddr += 4;
			Length -=4;
			data++;
		}
		else{
			HAL_FLASH_Lock();
			return Length;
		}
		// 等待FLASH总线空闲
        if(FLASH_WaitForLastOperation(HAL_MAX_DELAY) != HAL_OK)
        {
            HAL_FLASH_Lock();
            return -1;
        }
	}
	HAL_FLASH_Lock();
	return 0;
}

int Drv_Flash_Read(unsigned int PageAddr, unsigned char *pData, int Length)
{
	while(Length > 0)
    {
        *pData = *(__IO unsigned char*)PageAddr;
        pData++;
        Length--;
        PageAddr++;
    }
	return 0;
}
