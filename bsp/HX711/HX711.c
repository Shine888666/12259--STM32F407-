
#include "HX711.h"
#include "stdio.h"
#include "board.h"



unsigned int HX711_Buffer;
unsigned int Weight_Maopi;
int Weight_Shiwu;
unsigned char Flag_Error = 0;

//У׼����
//��Ϊ��ͬ�Ĵ������������߲��Ǻ�һ�£���ˣ�ÿһ����������Ҫ�������������������ʹ����ֵ��׼ȷ��
//�����ֲ��Գ���������ƫ��ʱ�����Ӹ���ֵ��
//������Գ���������ƫСʱ����С����ֵ��
//��ֵ����ΪС��
#define GapValue 207.00


/******************************************************************
 * �� �� �� �ƣ�HX711_GPIO_Init
 * �� �� ˵ ����HX711�����ų�ʼ��
 * �� �� �� �Σ���
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע����
******************************************************************/
void HX711_GPIO_Init(void)
{
        GPIO_InitTypeDef  GPIO_InitStructure;

        RCC_AHB1PeriphClockCmd (RCC_HX711, ENABLE); // ʹ��GPIOBʱ��

        GPIO_InitStructure.GPIO_Pin = GPIO_DT|GPIO_SCK;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(PORT_HX711, &GPIO_InitStructure);
}

/******************************************************************
 * �� �� �� �ƣ�HX711_Read
 * �� �� ˵ ������ȡHX711
 * �� �� �� �Σ���
 * �� �� �� �أ���ȡ����ֵ
 * ��       �ߣ�LC
 * ��       ע����
******************************************************************/
unsigned int HX711_Read(void)        //����128
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
        count=count^0x800000;//��25�������½�����ʱ��ת������
        delay_us(1);
        SCK(0);
        return(count);
}

/******************************************************************
 * �� �� �� �ƣ�Get_Maopi
 * �� �� ˵ ����������ʼ����
 * �� �� �� �Σ���
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע�����������������Ըó�ʼ����Ϊ0ֵ������ڳ�ʼ��ʱ�����ϲ�Ҫ���κζ���
******************************************************************/
void Get_Maopi(void)
{
        Weight_Maopi = HX711_Read();
}
/******************************************************************
 * �� �� �� �ƣ�Get_Weight
 * �� �� ˵ ��������
 * �� �� �� �Σ���
 * �� �� �� �أ�����ֵ����λg
 * ��       �ߣ�LC
 * ��       ע����
******************************************************************/
float Get_Weight(void)
{
    float Weight=0;
        HX711_Buffer = HX711_Read();
        if(HX711_Buffer > Weight_Maopi)
        {
        Weight_Shiwu = HX711_Buffer - Weight_Maopi; //��ȡʵ���AD������ֵ��
        Weight = (float)Weight_Shiwu / (float)GapValue;//����ʵ���ʵ������
        //��Ϊ��ͬ�Ĵ������������߲�һ������ˣ�ÿһ����������Ҫ���������GapValue���������
        //�����ֲ��Գ���������ƫ��ʱ�����Ӹ���ֵ��
        //������Գ���������ƫСʱ����С����ֵ��
        }
    return Weight;
}


