/***********************************************************************
文件名称：main.C
功    能：板子上的 3个LED轮流闪烁 
实验平台：基于STM32F407VCT6 开发板
库版本  ：V1.2.1 
***********************************************************************/
#include "main.h"	

void Delay(__IO u32 nCount); 	
int main(void)
{
	/*
		系统时钟配置在system_stm32f4xx.c 文件中的SystemInit()函数中实现，在复位后直接在启动文件中运行
	*/
		SCB->VTOR = FLASH_BASE | 0x10000;//设置偏移量
	LED_GPIO_Config(); //LED 端口初始化 
	while (1)
	{
		LED1_ON;			  // 亮
		Delay(0x600000);
		LED1_OFF;		    // 灭

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

