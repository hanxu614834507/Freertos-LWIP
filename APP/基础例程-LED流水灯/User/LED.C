/*********************************************************************
 * 文件名  ：led.c
 * 描述    ：led 应用函数库
 *          
 * 实验平台：基于STM32F107VCT6 开发板
 * 硬件连接：-----------------
 *          |   PE13 - LED1   |
 *          |   PE14 - LED2   |
 *          |   PE15 - LED3   |
 *           ----------------- 
 * 库版本  ：V1.2.1 		
***********************************************************************/
#include "main.h"
/***************  配置LED用到的I/O口 *******************/
void LED_GPIO_Config(void)
{

	GPIO_InitTypeDef  GPIO_InitStructure;
	/* Enable the GPIO_LED Clock */
	RCC_AHB1PeriphClockCmd(  RCC_AHB1Periph_GPIOE, ENABLE); 		
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14| GPIO_Pin_15 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	/*初始化完后，关闭3个LED*/ 
	LED1_OFF;
	LED2_OFF;
	LED3_OFF;
}

