#ifndef __DRV_FLASH_H
#define __DRV_FLASH_H

int Drv_Flash_Erase(unsigned int PageAddr, unsigned short count);
int Drv_Flash_Write(unsigned int PageAddr, unsigned char *pData, int Length);
int Drv_Flash_Read(unsigned int PageAddr, unsigned char *pData, int Length);

#endif /* __DRV_FLASH_H */
