#ifndef _LED_H_
#define _LED_H_

#define LED1_ON 		GPIO_ResetBits(GPIOB , GPIO_Pin_14)
#define LED2_ON 		GPIO_ResetBits(GPIOC , GPIO_Pin_13)
#define LED3_ON 		GPIO_ResetBits(GPIOC , GPIO_Pin_0)



#define LED1_OFF 		GPIO_SetBits(GPIOB , GPIO_Pin_14)
#define LED2_OFF 		GPIO_SetBits(GPIOC , GPIO_Pin_13)
#define LED3_OFF 		GPIO_SetBits(GPIOC , GPIO_Pin_0)


void LED_GPIO_Config(void);

#endif
