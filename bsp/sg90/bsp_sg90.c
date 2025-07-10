
#include "bsp_sg90.h"

unsigned int Servo_Angle = 0;//����Ƕ�

/******************************************************************
 * �� �� �� �ƣ�SG90_Init
 * �� �� ˵ ����PWM����
 * �� �� �� �Σ� pre��ʱ��ʱ��Ԥ��Ƶֵ    per����
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע��PWMƵ��=84 000 000 /( (pre+1) * (per+1) ) 
 
����ռ�ձ� ��Χ 0 ~ (per-1)
//    t = 0.5ms������������-�����ת�� 0 ��
//    t = 1.0ms������������-�����ת�� 45��
//    t = 1.5ms������������-�����ת�� 90��
//    t = 2.0ms������������-�����ת�� 135��
//    t = 2.5ms������������-�����ת��180��
//    1.�Ҹ���μ���������������Ӷ�ʵ������Ҫ�Ĺ��ܣ�
//    �˴���90��Ϊ����
//    ��PWM����Ϊ20ms������ռ�ձȾ�Ӧ��Ϊ1.5ms/20ms = 7.5%��
//    ���� TIMx ����Ƚ� 1 �Ĵ���ֵ��Ϊperiod-period*7.5% = 84-84*7.5%=77.7
******************************************************************/
void SG90_Init(void)
{       
	/* ���ö�ʱ������ */
	// Ƶ��f =ϵͳʱ�� / ( (prescaler+1) * (period+1) ) 
	// Ƶ��f = 84,000,000/ (8400 * 200)  = 50hz 
	// ����T = 1/f = 1/50 = 0.02S = 20ms

	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_SIG_TIMER, ENABLE);	//ʹ�ܶ�ʱ��3ʱ��
 	RCC_AHB1PeriphClockCmd(RCC_SIG, ENABLE);   //ʹ��GPIO����
	   
   //���ø�����Ϊ�����������,���TIM3 
	GPIO_InitStructure.GPIO_Pin = GPIO_SIG; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(PORT_SIG, &GPIO_InitStructure);//��ʼ��GPIO
	
	GPIO_PinAFConfig(PORT_SIG, GPIO_SIG_SOURCE, AF_SIG); //����
 
	//��ʼ��TIM3
	//����ռ�ձ� ��Χ 0 ~ (per-1)
	//    t = 0.5ms������������-�����ת�� 0 ��
	//    t = 1.0ms������������-�����ת�� 45��
	//    t = 1.5ms������������-�����ת�� 90��
	//    t = 2.0ms������������-�����ת�� 135��
	//    t = 2.5ms������������-�����ת��180��
	//    1.�Ҹ���μ���������������Ӷ�ʵ������Ҫ�Ĺ��ܣ�
	//    �˴���90��Ϊ����
	//    ��PWM����Ϊ20ms������ռ�ձȾ�Ӧ��Ϊ1.5ms/20ms = 7.5%��
	//    ���� TIMx ����Ƚ� 1 �Ĵ���ֵ��Ϊperiod-period*7.5% = 84-84*7.5%=77.7
	TIM_TimeBaseStructure.TIM_Period = 200-1; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =8400-1; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // ʱ�ӷ�Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(BSP_SIG_TIMER, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	//��ʼ��TIM3 Channel1 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������
	
	TIM_OC1Init(BSP_SIG_TIMER, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC1
 
	TIM_OC1PreloadConfig(BSP_SIG_TIMER, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR1�ϵ�Ԥװ�ؼĴ���
 
	TIM_Cmd(BSP_SIG_TIMER, ENABLE);  //ʹ��TIM3
}


/******************************************************************
 * �� �� �� �ƣ�Set_Servo_Angle
 * �� �� ˵ �������ýǶ�
 * �� �� �� �Σ�angle=Ҫ���õĽǶȣ���Χ0-180
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע����
******************************************************************/
void Set_Servo_Angle(unsigned int angle)
{
    if(angle > 180)
    {
        angle = 180; // ���ƽǶ���0��180��֮��
    }

    // ����PWMռ�ձ�
    // ע�⣺�����200�ǻ���TIM_Period����Ϊ199�ģ�200-1��
    // 0.5ms��Ӧ�ļ��� = (0.5 / 20) * 200
    // 2.5ms��Ӧ�ļ��� = (2.5 / 20) * 200
    // ��ˣ��Ƕȴ�0��180��ʱ��ռ�ձ�Ӧ�ô�0.5ms��2.5ms���Ա仯
    unsigned int ServoAngle = (unsigned int)((0.5 + (angle / 180.0) * 2.0) / 20.0 * 200);

    TIM_SetCompare1(BSP_SIG_TIMER, ServoAngle);
}


/******************************************************************
 * �� �� �� �ƣ���ȡ��ǰ�Ƕ�
 * �� �� ˵ ����Get_Servo_Angle
 * �� �� �� �Σ���
 * �� �� �� �أ���ǰ�Ƕ�
 * ��       �ߣ�LC
 * ��       ע��ʹ��ǰ����ȷ��֮ǰʹ�ù� 
                void Set_Servo_Angle(unsigned int angle) 
                �������ù��Ƕ�
******************************************************************/
unsigned int Get_Servo_Angle(void)
{
        return Servo_Angle;
}