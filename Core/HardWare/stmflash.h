#ifndef 		__STMFLASH_H_
#define  		__STMFLASH_H_

#include	"main.h"

#define		STM32_FLASH_BASE		FLASH_BASE
#define 	STM32_FLASH_SIZE		0x100000
#define FLASH_WAITETIME       50000           /* FLASH等待超时时间 */

/* F4  FLASH 扇区的起始地址 */
#define FLASH_SCETOR0_ADDR		((uint32_t)	0x08000000)		//16k
#define FLASH_SCETOR1_ADDR		((uint32_t)	0x08004000)		//16k
#define FLASH_SCETOR2_ADDR		((uint32_t)	0x08008000)		//16k
#define FLASH_SCETOR3_ADDR		((uint32_t)	0x0800C000)		//16k
#define FLASH_SCETOR4_ADDR		((uint32_t)	0x08010000)		//64k
#define FLASH_SCETOR5_ADDR		((uint32_t)	0x08020000)		//128k
#define FLASH_SCETOR6_ADDR		((uint32_t)	0x08040000)		//128k
#define FLASH_SCETOR7_ADDR		((uint32_t)	0x08060000)		//128k
#define FLASH_SCETOR8_ADDR		((uint32_t)	0x08080000)		//128k
#define FLASH_SCETOR9_ADDR		((uint32_t)	0x080A0000)		//128k
#define FLASH_SCETOR10_ADDR		((uint32_t)	0x080C0000)		//128k
#define FLASH_SCETOR11_ADDR		((uint32_t)	0x080E0000)		//128k

uint32_t STMFLASH_ReadWord(uint32_t faddr);
void stmflash_read(uint32_t raddr, uint32_t *pbuf, uint32_t length);
uint8_t stmflash_get_flash_sector(uint32_t addr);
void StmFlash_Write(uint32_t WriteAddr,uint32_t *pBuffer ,uint32_t  Buf_Length);


#endif 

