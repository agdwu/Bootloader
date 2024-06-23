#ifndef		__BOOTLOADER_H
#define		__BOOTLOADER_H

#include "main.h"
#include "stmflash.h"
#include "usart.h"

#define		BOOTLOADER_SIZE				100*1024			//100KB
#define 	APP_ADDR        			0x08019000     //��һ��Ӧ�ó�����ʼ��ַ(������ڲ�FLASH)

#define PackageSize						1024


typedef  void (*PFunc)(void);							
typedef  void (*PFunc_U16)(uint16_t);	

typedef struct
{
	uint8_t			UpdataStart;		//��ʼ���±�־
	uint8_t			UpdataFinish;		//������ɱ�־
	uint16_t		PackageIndex;		//�����
	uint16_t 		PackageNum;			//�ܰ���
	uint16_t		PacketSize;			//���ݰ���С
	uint8_t*		RxBuff;					//��������ָ��
	uint8_t*		TxBuff;					//��������ָ��
	uint16_t*		RxLength;				//�������ݳ���
	PFunc_U16		SendFunc;				//���ͺ���ָ�� 

}BootloaderStruct;

extern BootloaderStruct IAP_Bootloder;

void IAP_Write_APPbin(uint32_t  APP_Addr, uint8_t *APP_Buf, uint32_t  APP_Size);
void IAP_load_APP(uint32_t	APP_Addr);



#endif
