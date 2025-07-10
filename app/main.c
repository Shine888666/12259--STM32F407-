#include "board.h"
#include "bsp_uart.h"
#include <stdio.h>
#include "oled.h"
#include "HX711.h"
#include "BSP_SG90.H"



int a=0;
int RxData=0;
int weight=0;



int main(void)
{
	
	
	
	float t = 0.0;
	int count = 0;
		
	board_init();
	OLED_Init();
	OLED_Clear();
	uart1_init(9600U);
	
	HX711_GPIO_Init();
	SG90_Init();
    
	Get_Maopi();  //��ëƤ����
    delay_ms(500);
    Get_Maopi();  //���»�ȡëƤ����
	
	printf("USART Init!!\r\n");
	
	while(1)
	{
		weight=Get_Weight();
		if (Serial_GetRxFlag() == 1)			//��鴮�ڽ������ݵı�־λ
		{
			printf("ok\n");
			RxData = Serial_GetRxData()-'0';		//��ȡ���ڽ��յ�����
			printf("%d",RxData);
		}
		if (RxData==1 && weight<250)
		{
			Set_Servo_Angle(120);
			delay_ms(500);
			Set_Servo_Angle(150);
//			delay_ms(1000);
		}
		else{
			Set_Servo_Angle(150);
			RxData=0;
			
		}
		OLED_ShowNum(0,0,RxData,3,24,1);
		OLED_ShowNum(0,25,weight,5,24,1);
        OLED_Refresh();

	}
	
}


