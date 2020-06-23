/***********************************************************************
�ļ����ƣ�main.C
��    �ܣ������ϵ� 3��LED������˸ 
ʵ��ƽ̨������STM32F407VCT6 ������
��汾  ��V1.2.1 
***********************************************************************/
#include "main.h"	

void Delay(__IO u32 nCount); 	
int main(void)
{
	/*
		ϵͳʱ��������system_stm32f4xx.c �ļ��е�SystemInit()������ʵ�֣��ڸ�λ��ֱ���������ļ�������
	*/
		SCB->VTOR = FLASH_BASE | 0x10000;//����ƫ����
	LED_GPIO_Config(); //LED �˿ڳ�ʼ�� 
	while (1)
	{
		LED1_ON;			  // ��
		Delay(0x600000);
		LED1_OFF;		    // ��

		LED2_ON;
		Delay(0x600000);
		LED2_OFF;

		LED3_ON;
		Delay(0x600000);
		LED3_OFF; 
	}
}

void Delay(__IO u32 nCount)
{
  for(; nCount != 0; nCount--) ;
}

