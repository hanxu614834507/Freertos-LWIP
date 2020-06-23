#ifndef _LED_H_
#define _LED_H_

#define LED1_ON 		GPIO_ResetBits(GPIOE , GPIO_Pin_13)
#define LED2_ON 		GPIO_ResetBits(GPIOE , GPIO_Pin_14)
#define LED3_ON 		GPIO_ResetBits(GPIOE , GPIO_Pin_15)



#define LED1_OFF 		GPIO_SetBits(GPIOE , GPIO_Pin_13)
#define LED2_OFF 		GPIO_SetBits(GPIOE , GPIO_Pin_14)
#define LED3_OFF 		GPIO_SetBits(GPIOE , GPIO_Pin_15)


void LED_GPIO_Config(void);

#endif
