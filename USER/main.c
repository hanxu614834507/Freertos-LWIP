#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "timer.h"
#include "lcd.h"
#include "key.h"
#include "beep.h"
#include "string.h"
#include "malloc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "lwip_comm.h" 
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/init.h"
#include "ethernetif.h" 
#include "lwip/tcpip.h" 
#include "tcp_client_demo.h"
#include "bsp_spi_flash.h"
#include "iap.h"
/************************************************
  文 件 名   : main.c
  版 本 号   : 初稿
  作    者   : 韩旭
  生成日期   : 2020年6月16日
  最近修改   :
  功能描述   : 主程序模块
  函数列表   :
  修改历史   :
  1.日    期   : 2020年6月16日
    作    者   : 韩旭
    修改内容   : 创建文件
************************************************/

//任务优先级
#define START_TASK_PRIO		(tskIDLE_PRIORITY)
//任务堆栈大小	
#define START_STK_SIZE 		2*2048
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define TASK1_TASK_PRIO		(tskIDLE_PRIORITY+1)
//任务堆栈大小	
#define TASK1_STK_SIZE 		1024  
//任务句柄
TaskHandle_t Task1Task_Handler;
//任务函数
void task1_task(void *pvParameters);

//任务优先级
#define TASK2_TASK_PRIO 	(tskIDLE_PRIORITY+1)
//任务堆栈大小	
#define TASK2_STK_SIZE 		1024
//任务句柄
TaskHandle_t Task2Task_Handler;
//任务函数
void task2_task(void *pvParameters);
SemaphoreHandle_t BinarySemaphore;	//二值信号量句柄
////任务优先级
//#define TASK3_TASK_PRIO 3
////任务堆栈大小	
//#define TASK3_STK_SIZE  256 
////任务句柄
//TaskHandle_t Task3Task_Handler;
////任务函数
//void task3_task(void *pvParameters);
typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;
//用于命令解析用的命令值
#define LED1ON	1
#define LED1OFF	2
#define BEEPON	3
#define BEEPOFF	4
#define COMMANDERR	0XFF
#define TRANSHEAD_START_ADD 0x1E0000 //外部flash存储地址
//进入低功耗模式前需要处理的事情
void BeforeEnterSleep(void)
{
	//关闭某些低功耗模式下不使用的外设时钟，
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, DISABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, DISABLE);	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, DISABLE);	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, DISABLE);	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, DISABLE);	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, DISABLE);	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, DISABLE);	  
}

//退出低功耗模式以后需要处理的事情
void AfterExitSleep(void)
{
	//退出低功耗模式以后打开那些被关闭的外设时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);	              
}

//空闲任务钩子函数
//void vApplicationIdleHook(void)
//{
//	__disable_irq();
//	__dsb(portSY_FULL_READ_WRITE );
//	__isb(portSY_FULL_READ_WRITE );
//	
//	BeforeEnterSleep();		//进入睡眠模式之前需要处理的事情
//	__wfi();				//进入睡眠模式
//	AfterExitSleep();		//退出睡眠模式之后需要处理的事情
//	
//	__dsb(portSY_FULL_READ_WRITE );
//	__isb(portSY_FULL_READ_WRITE );
//	__enable_irq();
//}

//将字符串中的小写字母转换为大写
//str:要转换的字符串
//len：字符串长度
void LowerToCap(u8 *str,u8 len)
{
	u8 i;
	for(i=0;i<len;i++)
	{
		if((96<str[i])&&(str[i]<123))	//小写字母
		str[i]=str[i]-32;				//转换为大写
	}
}

//命令处理函数，将字符串命令转换成命令值
//str：命令
//返回值: 0XFF，命令错误；其他值，命令值
u8 CommandProcess(u8 *str)
{
	u8 CommandValue=COMMANDERR;
	if(strcmp((char*)str,"LED1ON")==0) CommandValue=LED1ON;
	else if(strcmp((char*)str,"LED1OFF")==0) CommandValue=LED1OFF;
	else if(strcmp((char*)str,"BEEPON")==0) CommandValue=BEEPON;
	else if(strcmp((char*)str,"BEEPOFF")==0) CommandValue=BEEPOFF;
	return CommandValue;
}

int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
	delay_init(168);					//初始化延时函数
	uart_init(115200);     				//初始化串口
	LED_Init();		        			//初始化LED端口
	KEY_Init();							//初始化按键
	my_mem_init(SRAMIN);            	//初始化内部内存池
	my_mem_init(SRAMEX);								//初始化外部内存池
	my_mem_init(SRAMCCM);	  					    //初始化CCM内存池
	
	
	while(lwip_comm_init())					//lwip初始化
	{
			printf("Lwip Init failed!\r\n");//初始化LWIP失败
			
	}
			printf("Lwip Init success！\r\n");//初始化LWIP成功
	while(tcp_client_init())							//初始化tcp_client(创建tcp_client线程)
	{
			printf("TCP Client failed!!");			//tcp客户端创建失败
			
	}
			printf("TCP Client success!!");			 //tcp客户端创建成功
	//创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
}

//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
	BinarySemaphore=xSemaphoreCreateBinary();	
    //创建TASK1任务
    xTaskCreate((TaskFunction_t )task1_task,             
                (const char*    )"task1_task",           
                (uint16_t       )TASK1_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )TASK1_TASK_PRIO,        
                (TaskHandle_t*  )&Task1Task_Handler);   
    //创建TASK2任务
    xTaskCreate((TaskFunction_t)task2_task,
									(const char *)"task2_task",
									(uint16_t)TASK2_STK_SIZE,
									(void *)NULL,
									(UBaseType_t)TASK2_TASK_PRIO,
									(TaskHandle_t *)&Task2Task_Handler);

    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//task1任务函数
void task1_task(void *pvParameters)
{
	while(1)
	{
		LED1=!LED1;
		//printf("tocuh");
		//LED0=!LED0;
    vTaskDelay(500);             	//延时500ms，也就是500个时钟节拍	
	}
}


void task2_task(void *pvParameters)//在线更新iap任务
{
	
	while(1)
	{
	if(KEY_Scan(0))
		{
			if(KEY0 == 0)
			{
					taskENTER_CRITICAL(); //关中断
					taskENTER_CRITICAL();							//进入临界区
				
				printf("您未按下升级按键KEY_UP!设备将尝试正常运行! \r\n");
					printf("- - - - 正在跳转 - - - - \r\n");
				/* Check if valid stack address (RAM address) then jump to user application */
	  /* 检查栈顶地址信息是否合法，合法则跳转至app */
				if(((*(vu32*)(USER_FLASH_FIRST_PAGE_ADDRESS+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
		{
				if (((*(vu32*)USER_FLASH_FIRST_PAGE_ADDRESS) & 0x2FFE0000 ) == 0x20000000)
			{
					printf("正常运行中! \r\n");
					/* Jump to user application */
					JumpAddress = *(__IO uint32_t*) (USER_FLASH_FIRST_PAGE_ADDRESS + 4);
					Jump_To_Application = (pFunction) JumpAddress;
					/* Initialize user application's Stack Pointer */
					__set_MSP(*(__IO uint32_t*) USER_FLASH_FIRST_PAGE_ADDRESS);
					Jump_To_Application();
					/* do nothing */
			}
		}
	//如果flash栈信息中没有app代码，则打印反馈信息
			else
		{
			printf("FLASH中没有app程序! \r\n");
		}
		taskEXIT_CRITICAL();            //退出临界区
					taskEXIT_CRITICAL();   //开中断
		}
				
	}	
}
}	

//DataProcess_task函数
//void DataProcess_task(void *pvParameters)
//{
//	u8 len=0;
//	u8 CommandValue=COMMANDERR;
//	BaseType_t err=pdFALSE;
//	
//	u8 *CommandStr;
//	while(1)
//	{
//		err=xSemaphoreTake(BinarySemaphore,portMAX_DELAY);	//获取信号量
//		if(err==pdTRUE)										//获取信号量成功
//		{
//			len=USART_RX_STA&0x3fff;						//得到此次接收到的数据长度
//			CommandStr=mymalloc(SRAMIN,len+1);				//申请内存
//			sprintf((char*)CommandStr,"%s",USART_RX_BUF);
//			CommandStr[len]='\0';							//加上字符串结尾符号
//			LowerToCap(CommandStr,len);						//将字符串转换为大写		
//			CommandValue=CommandProcess(CommandStr);		//命令解析
//			if(CommandValue!=COMMANDERR)
//			{
//				printf("命令为:%s\r\n",CommandStr);
//				switch(CommandValue)						//处理命令
//				{
//					case LED1ON: 
//						LED1=0;
//						break;
//					case LED1OFF:
//						LED1=1;
//						break;
//					case BEEPON:
//						BEEP=1;
//						break;
//					case BEEPOFF:
//						BEEP=0;
//						break;
//				}
//			}
//			else
//			{
//				printf("无效的命令，请重新输入!!\r\n");
//			}
//			USART_RX_STA=0;
//			memset(USART_RX_BUF,0,USART_REC_LEN);			//串口接收缓冲区清零
//			myfree(SRAMIN,CommandStr);						//释放内存
//		}
//	}
//}

