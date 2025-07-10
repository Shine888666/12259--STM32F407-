
#ifndef __HX711_H_
#define __HX711_H_

#include "stm32f4xx.h"

//�˿���ֲ
#define RCC_HX711         RCC_AHB1Periph_GPIOB
#define PORT_HX711         GPIOB

#define GPIO_SCK         GPIO_Pin_8
#define GPIO_DT         GPIO_Pin_9

//����DT���ģʽ
#define DT_OUT()     {        \
                        GPIO_InitTypeDef  GPIO_InitStructure; \
                        GPIO_InitStructure.GPIO_Pin = GPIO_DT; \
                        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; \
                        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; \
                        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; \
                        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; \
                        GPIO_Init(PORT_HX711, &GPIO_InitStructure); \
                     }
//����DT����ģʽ
#define DT_IN()      {        \
                        GPIO_InitTypeDef  GPIO_InitStructure; \
                        GPIO_InitStructure.GPIO_Pin = GPIO_DT; \
                        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; \
                        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; \
                        GPIO_Init(PORT_HX711, &GPIO_InitStructure); \
                    }
//��ȡDT���ŵĵ�ƽ�仯
#define DT_GET()        GPIO_ReadInputDataBit(PORT_HX711, GPIO_DT)
//DT��SCK���
#define DT(x)           GPIO_WriteBit(PORT_HX711,GPIO_DT, (x?Bit_SET:Bit_RESET))
#define SCK(x)          GPIO_WriteBit(PORT_HX711,GPIO_SCK, (x?Bit_SET:Bit_RESET))

void HX711_GPIO_Init(void);
float Get_Weight(void);
void Get_Maopi(void);
					
#endif
					
	
					
					