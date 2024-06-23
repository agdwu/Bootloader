#ifndef		__BOOTLOADER_H
#define		__BOOTLOADER_H

#include "main.h"
#include "stmflash.h"
#include "usart.h"

#define		BOOTLOADER_SIZE				100*1024			//100KB
#define 	APP_ADDR        			0x08019000     //第一个应用程序起始地址(存放在内部FLASH)

#define PackageSize						1024


typedef  void (*PFunc)(void);							
typedef  void (*PFunc_U16)(uint16_t);	

typedef struct
{
	uint8_t			UpdataStart;		//开始更新标志
	uint8_t			UpdataFinish;		//更新完成标志
	uint16_t		PackageIndex;		//包序号
	uint16_t 		PackageNum;			//总包数
	uint16_t		PacketSize;			//数据包大小
	uint8_t*		RxBuff;					//接收数据指针
	uint8_t*		TxBuff;					//发送数据指针
	uint16_t*		RxLength;				//接收数据长度
	PFunc_U16		SendFunc;				//发送函数指针 

}BootloaderStruct;

extern BootloaderStruct IAP_Bootloder;

void IAP_Write_APPbin(uint32_t  APP_Addr, uint8_t *APP_Buf, uint32_t  APP_Size);
void IAP_load_APP(uint32_t	APP_Addr);



#endif
