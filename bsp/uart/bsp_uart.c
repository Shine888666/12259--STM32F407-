
#include "bsp_uart.h" 
#include "stdio.h"
uint8_t Serial_RxData;		//���崮�ڽ��յ����ݱ���
uint8_t Serial_RxFlag;		//���崮�ڽ��յı�־λ����
void uart1_init(uint32_t __Baud)
{
	GPIO_InitTypeDef GPIO_InitStructure;	

	RCC_AHB1PeriphClockCmd(BSP_USART_TX_RCC,ENABLE);
	RCC_AHB1PeriphClockCmd(BSP_USART_RX_RCC,ENABLE);

	//IO��������������Ҫ���ø���ģʽ
	GPIO_PinAFConfig(BSP_USART_TX_PORT,BSP_USART_TX_AF_PIN,BSP_USART_AF);
	GPIO_PinAFConfig(BSP_USART_RX_PORT,BSP_USART_RX_AF_PIN,BSP_USART_AF);

	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin           = BSP_USART_TX_PIN;	//TX����
	GPIO_InitStructure.GPIO_Mode          = GPIO_Mode_AF;		//IO��������������Ҫ���ø���ģʽ
	GPIO_InitStructure.GPIO_Speed         = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType         = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd          = GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin           = BSP_USART_RX_PIN;	//RX����
	GPIO_InitStructure.GPIO_Mode          = GPIO_Mode_AF;		//IO��������������Ҫ���ø���ģʽ
	GPIO_InitStructure.GPIO_Speed         = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType         = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd          = GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
  
	USART_InitTypeDef USART_InitStructure;//�������ô��ڵĽṹ�����

	RCC_APB2PeriphClockCmd(BSP_USART_RCC, ENABLE);//��������1��ʱ��

	USART_DeInit(BSP_USART);//�����˼�ǽ���˴��ڵ���������

	USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate              = __Baud;//���ò�����
	USART_InitStructure.USART_WordLength            = USART_WordLength_8b;//�ֽڳ���Ϊ8bit
	USART_InitStructure.USART_StopBits              = USART_StopBits_1;//1��ֹͣλ
	USART_InitStructure.USART_Parity                = USART_Parity_No ;//û��У��λ
	USART_InitStructure.USART_Mode                  = USART_Mode_Rx | USART_Mode_Tx;//����������Ϊ�շ�ģʽ
	USART_InitStructure.USART_HardwareFlowControl   = USART_HardwareFlowControl_None; //���ṩ���� 
	USART_Init(BSP_USART,&USART_InitStructure);//����ز�����ʼ��������1
	
	USART_ClearFlag(BSP_USART,USART_FLAG_RXNE);//��ʼ����ʱ���������λ
	
	USART_Cmd(BSP_USART,ENABLE);//��������1

	USART_ITConfig(BSP_USART, USART_IT_RXNE, ENABLE);//��ʼ���ý����ж�

	
	NVIC_InitTypeDef NVIC_InitStructure;//�жϿ��ƽṹ���������

	NVIC_InitStructure.NVIC_IRQChannel                    = USART1_IRQn;//�ж�ͨ��ָ��ΪUSART1
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority  = 0;//�����ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority         = 1;//�����ȼ�Ϊ1
	NVIC_InitStructure.NVIC_IRQChannelCmd                 = ENABLE;//ȷ��ʹ��
	NVIC_Init(&NVIC_InitStructure);//��ʼ�����ô��ж�ͨ��
		
}


void usart_send_data(uint8_t ucch)
{
    USART_SendData(BSP_USART, (uint8_t)ucch);
	
	// �ȴ��������ݻ�������־��λ
    while( RESET == USART_GetFlagStatus(BSP_USART, USART_FLAG_TXE) ){} 
}


void usart_send_String(uint8_t *ucstr)
{   
      while(ucstr && *ucstr)  // ��ַΪ�ջ���ֵΪ������   
      {     
			usart_send_data(*ucstr++);    
      }
}
uint8_t Serial_GetRxFlag(void)
{
	if (Serial_RxFlag == 1)			//�����־λΪ1
	{
		Serial_RxFlag = 0;
		return 1;					//�򷵻�1�����Զ������־λ
	}
	return 0;						//�����־λΪ0���򷵻�0
}

/**
  * ��    ������ȡ���ڽ��յ�����
  * ��    ������
  * �� �� ֵ�����յ����ݣ���Χ��0~255
  */
uint8_t Serial_GetRxData(void)
{
	return Serial_RxData;			//���ؽ��յ����ݱ���
}



#if !defined(__MICROLIB)
//��ʹ��΢��Ļ�����Ҫ�������ĺ���
#if (__ARMCLIB_VERSION <= 6000000)
//�����������AC5  �Ͷ�����������ṹ��
struct __FILE
{
	int handle;
};
#endif

FILE __stdout;

//����_sys_exit()�Ա���ʹ�ð�����ģʽ
void _sys_exit(int x)
{
	x = x;
}
#endif

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    USART_SendData(BSP_USART, (uint8_t)ch);
	
	while( RESET == USART_GetFlagStatus(BSP_USART, USART_FLAG_TXE) ){}
	
    return ch;
}

void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)		//�ж��Ƿ���USART1�Ľ����¼��������ж�
	{
		Serial_RxData = USART_ReceiveData(USART1);				//��ȡ���ݼĴ���������ڽ��յ����ݱ���
		Serial_RxFlag = 1;										//�ý��ձ�־λ����Ϊ1
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);			//���USART1��RXNE��־λ
																//��ȡ���ݼĴ������Զ�����˱�־λ
																//����Ѿ���ȡ�����ݼĴ�����Ҳ���Բ�ִ�д˴���
	}
}


