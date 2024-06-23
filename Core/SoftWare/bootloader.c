#include "bootloader.h"


BootloaderStruct IAP_Bootloder;
 

uint32_t IAP_Buffer[512];

void IAP_Write_APPbin(uint32_t  APP_Addr, uint8_t *APP_Buf, uint32_t  APP_Size)
{
	uint16_t	t;
	uint16_t	i=0;
	uint16_t	temp;
	uint32_t	fwaddr=APP_Addr;
	uint8_t		*dbuf=APP_Buf;
	
	for(t=0;t<APP_Size;t+=4)
	{
		temp=(uint32_t)dbuf[3] << 24;
		temp |= (uint32_t)dbuf[2] << 16;
		temp |= (uint32_t)dbuf[1] << 8;
    temp |= (uint32_t)dbuf[0];
		dbuf += 4;               /* ƫ��2���ֽ� */
		IAP_Buffer[i++] = temp;
		
		if(i==512)
		{
			i = 0;
			StmFlash_Write(fwaddr,IAP_Buffer,512);
			fwaddr+=2048;
		}
	}
	if(i)	StmFlash_Write(fwaddr,IAP_Buffer,i);
}

void IAP_load_APP(uint32_t	APP_Addr)
{
	PFunc	Jump2App;
	printf("ok");
	if(((*(volatile  uint32_t *)APP_Addr) & 0x2FFE0000)==0x20000000)
	{
		Jump2App=(PFunc)*(volatile uint32_t *)(APP_Addr+4);
		__set_MSP(*(volatile  uint32_t *)APP_Addr);	/* ����ջ����ַ */
//		for(int i=0;i<8;i++)
//		{
//			NVIC->ICER[i]=0xFFFFFFFF;	//�ر��ж�
//			NVIC->ICER[i]=0xFFFFFFFF;	//����жϱ�־λ
//		}
//		HAL_RCC_DeInit();
//		SysTick->CTRL=0;
//		SysTick->LOAD=0;
//		SysTick->VAL=0;
		
		Jump2App();
	}
}

//IAP��ʼ��
void IAP_BootLoad_Init(void)	
{
    //��ʼ��
    IAP_Bootloder.UpdataStart = 0;
    IAP_Bootloder.UpdataFinish = 0;
    IAP_Bootloder.RxBuff = UART4_Rx_Buff;//��������ָ��
    IAP_Bootloder.TxBuff = UART4_Tx_Buff;//��������ָ��
    IAP_Bootloder.RxLength = &Uart4_Rx_length;//���ճ���
    IAP_Bootloder.SendFunc = Uart4_CRC_Send;//���ͺ���
}

//	CRC16_Modbus
uint16_t  IAP_CRC16_Calculate(uint8_t *buff, uint16_t length)
{
    uint16_t i;
    uint16_t CrcInit = 0xFFFF;  //CRC16_modbus��ʼֵ
    uint16_t CrcPoly = 0xA001;  //CRC16_modbus����ʽ 8005���������ߵͻ�����������>A001

    for (i = 0; i < length; i++) 
    {
        CrcInit ^= buff[i];
        for (uint8_t j = 0; j < 8; j++) 
        {
            if (CrcInit & 0x0001) 
            {
                CrcInit = (CrcInit >> 1) ^ CrcPoly;
            }
            else {
                CrcInit >>= 1;
            }
        }
    }
    return CrcInit;
}

/*
��λ���뵥Ƭ��CRCУ��Ա�
1:�ɹ� 0:���� 
*/
uint8_t Crc_Check(uint8_t *pbuf,uint16_t pBufSize)
{
	uint32_t PC_crcVel=0;
	uint32_t MCU_crcvel = 0;
	uint8_t *buf;
	
	if(pBufSize<5) return 0;
	
	buf = pbuf+pBufSize+4;
	
	PC_crcVel = buf[0]|buf[1]<<8;
	MCU_crcvel = IAP_CRC16_Calculate(pbuf,pBufSize-4);
	
	if(MCU_crcvel==PC_crcVel)	return 1;
	else return 0;
}


/*���磬��λ�����ı�����55 AA �ܰ���120 ��ǰ����10 �����ֽ���04 00 data0 data1...  crcL crcH
(���crc����λ������Ĵ��ܰ������������һ���ֽڵ�У��ֵ����
�豸�յ���Ҳ�Լ���һ��У��ֵ���Լ���ĺ���λ�����һ�����ն���*/

//���¹̼�
void IAP_Update_Firmware(void)
{
	uint8_t		crc = 0;
	uint8_t		i,j;
	//��λ������ˢ�̼���mcu����̼�
	if((IAP_Bootloder.RxBuff[0] == 0x55) && (IAP_Bootloder.RxBuff[1] == 0xAA))
	{
		if(Crc_Check(IAP_Bootloder.RxBuff,*IAP_Bootloder.RxLength-2))//���ܰ������������һ���ֽڵ�У��
		{
			IAP_Bootloder.UpdataStart=1;
			IAP_Bootloder.PacketSize=IAP_Bootloder.RxBuff[4]<<8|IAP_Bootloder.RxBuff[5];//������С
			IAP_Bootloder.PackageNum=IAP_Bootloder.RxBuff[2];	//��ȡ�ܰ���
			
			IAP_Bootloder.TxBuff[0] = 0xF1;//��Ӧ֡ͷ
			IAP_Bootloder.TxBuff[1] = 0x81;//������
			
		  for(i=0;i<IAP_Bootloder.PacketSize-8;i++)
			{
				IAP_Bootloder.TxBuff[2+i]=IAP_Bootloder.RxBuff[6+i];
				j++;
			}
			
			crc=IAP_CRC16_Calculate(IAP_Bootloder.TxBuff,(IAP_Bootloder.PacketSize-8));//�Խ��յ�data����CRCУ��
			
			IAP_Bootloder.TxBuff[j+2]=crc>>8;
			IAP_Bootloder.TxBuff[j+3]=crc&0xff;
			
			IAP_Bootloder.SendFunc(j+4);
		}
	}
	else if((IAP_Bootloder.RxBuff[0]==0x55)&&(IAP_Bootloder.RxBuff[1]==0xBB))//���̼�д��flash
	{
		j=0;
		if(Crc_Check(IAP_Bootloder.RxBuff,*IAP_Bootloder.RxLength-2))
		{
			//��ȡ�������к�
			IAP_Bootloder.PackageIndex=IAP_Bootloder.RxBuff[3];
			IAP_Bootloder.PacketSize=IAP_Bootloder.RxBuff[4]<<8|IAP_Bootloder.RxBuff[5];//������С
			
			IAP_Write_APPbin(APP_ADDR+IAP_Bootloder.PacketSize,&IAP_Bootloder.RxBuff[6],*IAP_Bootloder.RxLength-8);
			
			IAP_Bootloder.TxBuff[0] = 0xF1;
			IAP_Bootloder.TxBuff[1] = 0x82;
			
		  for(i=0;i<IAP_Bootloder.PacketSize-8;i++)
			{
				IAP_Bootloder.TxBuff[2+i]=IAP_Bootloder.RxBuff[6+i];
				j++;
			}
			//���
			IAP_Bootloder.TxBuff[j+2] = 0x01; //��ȷ
			
			IAP_CRC16_Calculate(IAP_Bootloder.TxBuff,(IAP_Bootloder.PacketSize-8));
			
      IAP_Bootloder.TxBuff[j+3] = crc>>8;
      IAP_Bootloder.TxBuff[j+4] = crc&0xff;

			IAP_Bootloder.SendFunc(j+5);
      if(IAP_Bootloder.PackageIndex == (IAP_Bootloder.PackageNum-1))
      {
         //flashд�����
         IAP_Bootloder.UpdataFinish = 1;			               
      }			
		}
		else	//crcУ��ʧ�ܣ����ش�������
		{
			j=0;
       //��Ӧ
       IAP_Bootloder.TxBuff[0] = 0xF1;
       IAP_Bootloder.TxBuff[1] = 0x82;    
            
		  for(i=0;i<IAP_Bootloder.PacketSize-8;i++)
			{
				IAP_Bootloder.TxBuff[2+i]=IAP_Bootloder.RxBuff[6+i];
				j++;
			}
        //���
				IAP_Bootloder.TxBuff[j+2] = 0x02;	//����
				crc = IAP_CRC16_Calculate(IAP_Bootloder.TxBuff,5);
				IAP_Bootloder.TxBuff[j+3] = crc>>8;
				IAP_Bootloder.TxBuff[j+4] = crc&0xff;
			
				IAP_Bootloder.SendFunc(j+5);  			
		}
	}
}











 










