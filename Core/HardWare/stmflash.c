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
        pbuf[i] = STMFLASH_ReadWord(raddr);    /* ��ȡ4���ֽ�. */
        raddr += 4; /* ƫ��4���ֽ�. */
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
	
	HAL_FLASH_Unlock();             /* ���� */
	FLASH->ACR&=~(1<<10);
	
	addrx=WriteAddr;
	endaddr=WriteAddr+Buf_Length*4;
	
	if(addrx<0X1FFF0000)
		{
			while(addrx<endaddr)
			{
			if(STMFLASH_ReadWord(addrx)!=0XFFFFFFFF)
				{
					FLASH_EraseInit.TypeErase=FLASH_TYPEERASE_SECTORS;/* �������ͣ��������� */
					FLASH_EraseInit.Sector=stmflash_get_flash_sector(addrx);/* Ҫ���������� */
					FLASH_EraseInit.NbSectors=1;					/* Ҫ���������� */
					FLASH_EraseInit.VoltageRange=FLASH_VOLTAGE_RANGE_3; /* ��ѹ��Χ��VCC=2.7~3.6V֮��!! */
					
					if(HAL_FLASHEx_Erase(&FLASH_EraseInit,&sectorerror)!=HAL_OK) break;/* ���������� */
			}else{
				addrx+=4;
			}
			 FLASH_WaitForLastOperation(FLASH_WAITETIME);
		}
	}
	 FlashStatus=FLASH_WaitForLastOperation(FLASH_WAITETIME);             /* �ȴ��ϴβ������ */
	if (FlashStatus==HAL_OK){
        while (WriteAddr < endaddr){
            if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, WriteAddr, *pBuffer) != HAL_OK){
                break;              /* д���쳣 */
            }
            WriteAddr += 4;
            pBuffer++;
        }
    }
    FLASH->ACR |= 1 << 10;          /* FLASH��������,��������fetch */
    HAL_FLASH_Lock();               /* ���� */
}















