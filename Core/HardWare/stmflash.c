#include "stmflash.h"


FLASH_EraseInitTypeDef FLASH_EraseInit;


uint32_t STMFLASH_ReadWord(uint32_t faddr)
{
    return *(volatile uint32_t *)faddr;
}

void stmflash_read(uint32_t raddr, uint32_t *pbuf, uint32_t length)
{
    uint32_t i;

    for (i = 0; i < length; i++)
    {
        pbuf[i] = STMFLASH_ReadWord(raddr);    /* 读取4个字节. */
        raddr += 4; /* 偏移4个字节. */
    }
}

uint8_t stmflash_get_flash_sector(uint32_t addr)
{
    if (addr < FLASH_SCETOR1_ADDR) return FLASH_SECTOR_0;
    else if (addr < FLASH_SCETOR2_ADDR) return FLASH_SECTOR_1;
    else if (addr < FLASH_SCETOR3_ADDR) return FLASH_SECTOR_2;
    else if (addr < FLASH_SCETOR4_ADDR) return FLASH_SECTOR_3;
    else if (addr < FLASH_SCETOR5_ADDR) return FLASH_SECTOR_4;
    else if (addr < FLASH_SCETOR6_ADDR) return FLASH_SECTOR_5;
    else if (addr < FLASH_SCETOR7_ADDR) return FLASH_SECTOR_6;
    else if (addr < FLASH_SCETOR8_ADDR) return FLASH_SECTOR_7;
    else if (addr < FLASH_SCETOR9_ADDR) return FLASH_SECTOR_8;
    else if (addr < FLASH_SCETOR10_ADDR) return FLASH_SECTOR_9;
    else if (addr < FLASH_SCETOR11_ADDR) return FLASH_SECTOR_10;
    return FLASH_SECTOR_11;
}


void StmFlash_Write(uint32_t WriteAddr,uint32_t *pBuffer ,uint32_t  Buf_Length)
{
	FLASH_EraseInitTypeDef	FLASH_EraseInit;
	HAL_StatusTypeDef		FlashStatus=HAL_OK;
	
	uint32_t	addrx=0;
	uint32_t	endaddr=0;
	uint32_t 	sectorerror=0;
	
	if((WriteAddr<STM32_FLASH_BASE)||(WriteAddr>(STM32_FLASH_BASE+STM32_FLASH_SIZE))||(WriteAddr%4)) return;
	
	HAL_FLASH_Unlock();             /* 解锁 */
	FLASH->ACR&=~(1<<10);
	
	addrx=WriteAddr;
	endaddr=WriteAddr+Buf_Length*4;
	
	if(addrx<0X1FFF0000)
		{
			while(addrx<endaddr)
			{
			if(STMFLASH_ReadWord(addrx)!=0XFFFFFFFF)
				{
					FLASH_EraseInit.TypeErase=FLASH_TYPEERASE_SECTORS;/* 擦除类型，扇区擦除 */
					FLASH_EraseInit.Sector=stmflash_get_flash_sector(addrx);/* 要擦除的扇区 */
					FLASH_EraseInit.NbSectors=1;					/* 要擦除的扇区 */
					FLASH_EraseInit.VoltageRange=FLASH_VOLTAGE_RANGE_3; /* 电压范围，VCC=2.7~3.6V之间!! */
					
					if(HAL_FLASHEx_Erase(&FLASH_EraseInit,&sectorerror)!=HAL_OK) break;/* 发生错误了 */
			}else{
				addrx+=4;
			}
			 FLASH_WaitForLastOperation(FLASH_WAITETIME);
		}
	}
	 FlashStatus=FLASH_WaitForLastOperation(FLASH_WAITETIME);             /* 等待上次操作完成 */
	if (FlashStatus==HAL_OK){
        while (WriteAddr < endaddr){
            if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, WriteAddr, *pBuffer) != HAL_OK){
                break;              /* 写入异常 */
            }
            WriteAddr += 4;
            pBuffer++;
        }
    }
    FLASH->ACR |= 1 << 10;          /* FLASH擦除结束,开启数据fetch */
    HAL_FLASH_Lock();               /* 上锁 */
}















