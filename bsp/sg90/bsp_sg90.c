
#include "bsp_sg90.h"

unsigned int Servo_Angle = 0;//舵机角度

/******************************************************************
 * 函 数 名 称：SG90_Init
 * 函 数 说 明：PWM配置
 * 函 数 形 参： pre定时器时钟预分频值    per周期
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：PWM频率=84 000 000 /( (pre+1) * (per+1) ) 
 
配置占空比 范围 0 ~ (per-1)
//    t = 0.5ms——————-舵机会转动 0 °
//    t = 1.0ms——————-舵机会转动 45°
//    t = 1.5ms——————-舵机会转动 90°
//    t = 2.0ms——————-舵机会转动 135°
//    t = 2.5ms——————-舵机会转动180°
//    1.我该如何计算括号里的数，从而实现我想要的功能？
//    此处以90度为例。
//    答：PWM周期为20ms，所以占空比就应该为1.5ms/20ms = 7.5%，
//    所以 TIMx 捕获比较 1 寄存器值就为period-period*7.5% = 84-84*7.5%=77.7
******************************************************************/
void SG90_Init(void)
{       
	/* 配置定时器参数 */
	// 频率f =系统时钟 / ( (prescaler+1) * (period+1) ) 
	// 频率f = 84,000,000/ (8400 * 200)  = 50hz 
	// 周期T = 1/f = 1/50 = 0.02S = 20ms

	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_SIG_TIMER, ENABLE);	//使能定时器3时钟
 	RCC_AHB1PeriphClockCmd(RCC_SIG, ENABLE);   //使能GPIO外设
	   
   //设置该引脚为复用输出功能,输出TIM3 
	GPIO_InitStructure.GPIO_Pin = GPIO_SIG; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(PORT_SIG, &GPIO_InitStructure);//初始化GPIO
	
	GPIO_PinAFConfig(PORT_SIG, GPIO_SIG_SOURCE, AF_SIG); //复用
 
	//初始化TIM3
	//配置占空比 范围 0 ~ (per-1)
	//    t = 0.5ms——————-舵机会转动 0 °
	//    t = 1.0ms——————-舵机会转动 45°
	//    t = 1.5ms——————-舵机会转动 90°
	//    t = 2.0ms——————-舵机会转动 135°
	//    t = 2.5ms——————-舵机会转动180°
	//    1.我该如何计算括号里的数，从而实现我想要的功能？
	//    此处以90度为例。
	//    答：PWM周期为20ms，所以占空比就应该为1.5ms/20ms = 7.5%，
	//    所以 TIMx 捕获比较 1 寄存器值就为period-period*7.5% = 84-84*7.5%=77.7
	TIM_TimeBaseStructure.TIM_Period = 200-1; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =8400-1; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 时钟分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(BSP_SIG_TIMER, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM3 Channel1 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性
	
	TIM_OC1Init(BSP_SIG_TIMER, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC1
 
	TIM_OC1PreloadConfig(BSP_SIG_TIMER, TIM_OCPreload_Enable);  //使能TIM3在CCR1上的预装载寄存器
 
	TIM_Cmd(BSP_SIG_TIMER, ENABLE);  //使能TIM3
}


/******************************************************************
 * 函 数 名 称：Set_Servo_Angle
 * 函 数 说 明：设置角度
 * 函 数 形 参：angle=要设置的角度，范围0-180
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void Set_Servo_Angle(unsigned int angle)
{
    if(angle > 180)
    {
        angle = 180; // 限制角度在0到180度之间
    }

    // 计算PWM占空比
    // 注意：这里的200是基于TIM_Period设置为199的（200-1）
    // 0.5ms对应的计数 = (0.5 / 20) * 200
    // 2.5ms对应的计数 = (2.5 / 20) * 200
    // 因此，角度从0到180度时，占空比应该从0.5ms到2.5ms线性变化
    unsigned int ServoAngle = (unsigned int)((0.5 + (angle / 180.0) * 2.0) / 20.0 * 200);

    TIM_SetCompare1(BSP_SIG_TIMER, ServoAngle);
}


/******************************************************************
 * 函 数 名 称：读取当前角度
 * 函 数 说 明：Get_Servo_Angle
 * 函 数 形 参：无
 * 函 数 返 回：当前角度
 * 作       者：LC
 * 备       注：使用前必须确保之前使用过 
                void Set_Servo_Angle(unsigned int angle) 
                函数设置过角度
******************************************************************/
unsigned int Get_Servo_Angle(void)
{
        return Servo_Angle;
}