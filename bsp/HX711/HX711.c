
#include "HX711.h"
#include "stdio.h"
#include "board.h"



unsigned int HX711_Buffer;
unsigned int Weight_Maopi;
int Weight_Shiwu;
unsigned char Flag_Error = 0;

//校准参数
//因为不同的传感器特性曲线不是很一致，因此，每一个传感器需要矫正这里这个参数才能使测量值很准确。
//当发现测试出来的重量偏大时，增加该数值。
//如果测试出来的重量偏小时，减小改数值。
//该值可以为小数
#define GapValue 207.00


/******************************************************************
 * 函 数 名 称：HX711_GPIO_Init
 * 函 数 说 明：HX711的引脚初始化
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void HX711_GPIO_Init(void)
{
        GPIO_InitTypeDef  GPIO_InitStructure;

        RCC_AHB1PeriphClockCmd (RCC_HX711, ENABLE); // 使能GPIOB时钟

        GPIO_InitStructure.GPIO_Pin = GPIO_DT|GPIO_SCK;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(PORT_HX711, &GPIO_InitStructure);
}

/******************************************************************
 * 函 数 名 称：HX711_Read
 * 函 数 说 明：读取HX711
 * 函 数 形 参：无
 * 函 数 返 回：读取到的值
 * 作       者：LC
 * 备       注：无
******************************************************************/
unsigned int HX711_Read(void)        //增益128
{
        unsigned long count;
        unsigned char i;

        DT_OUT();
        delay_us(5);

        DT(1);
        delay_us(1);
        SCK(0);
        count=0;

        DT_IN();
        delay_us(5);

        while(DT_GET());
        for(i=0;i<24;i++)
        {
                SCK(1);
                count=count<<1;
                delay_us(1);
                SCK(0);
                if(DT_GET())
                                count++;
                delay_us(1);
        }
        SCK(1);
        count=count^0x800000;//第25个脉冲下降沿来时，转换数据
        delay_us(1);
        SCK(0);
        return(count);
}

/******************************************************************
 * 函 数 名 称：Get_Maopi
 * 函 数 说 明：测量初始重量
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：后续的重量都是以该初始重量为0值，因此在初始化时，秤上不要放任何东西
******************************************************************/
void Get_Maopi(void)
{
        Weight_Maopi = HX711_Read();
}
/******************************************************************
 * 函 数 名 称：Get_Weight
 * 函 数 说 明：称重
 * 函 数 形 参：无
 * 函 数 返 回：称重值，单位g
 * 作       者：LC
 * 备       注：无
******************************************************************/
float Get_Weight(void)
{
    float Weight=0;
        HX711_Buffer = HX711_Read();
        if(HX711_Buffer > Weight_Maopi)
        {
        Weight_Shiwu = HX711_Buffer - Weight_Maopi; //获取实物的AD采样数值。
        Weight = (float)Weight_Shiwu / (float)GapValue;//计算实物的实际重量
        //因为不同的传感器特性曲线不一样，因此，每一个传感器需要矫正这里的GapValue这个除数。
        //当发现测试出来的重量偏大时，增加该数值。
        //如果测试出来的重量偏小时，减小改数值。
        }
    return Weight;
}


