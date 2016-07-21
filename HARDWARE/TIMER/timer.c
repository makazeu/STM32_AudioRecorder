#include "timer.h"
#include "led.h"
#include "GUI.h"
#include "usart.h"
#include "adc.h"
#include "dac.h"
#include "mmc_sd.h"
#include "ui.h"
#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK ս��������
//��ʱ�� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/1/13
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 
u8 tim3_en;
extern __IO int32_t OS_TimeMS;
extern u8 state;
extern u8 buf_max;
extern u16 i,sec;
extern u16 adcx;
extern u8 *buf;
extern u8 *buf_read;
extern u16 temp;
extern u16 i_read,sec_read;

//��ʱ��3�жϷ������	 
void TIM3_IRQHandler(void)
{ 
	//LCD_ShowxNum(60,250,TIM3->CNT,4,16,0);
	if(TIM3->SR&0X0001)//����ж�
	{
		//LED1=!LED1;
		//PAout(15)=!PAout(15);//ʾ�����ۿ�
		//if (state) tim3_en=1;
		//else tim3_en=0;
		if (state == 1) {
			PAout(15)=!PAout(15);//ʾ�����ۿ�
			//adcx=Get_Adc_Average(ADC_CH1,10);		
			adcx=Get_Adc(ADC_CH1);
			buf[i]=adcx>>4;
			if (buf[i]>buf_max) buf_max=buf[i];
			i++;
			//LED0 = 0;
			if (i==512){
				SD_WriteDisk(buf,sec,1);
				sec++;
				i=0;
				//Update UI
				GUI_UpdateSec();
			}
		}
		else if (state == 2){
			if (sec_read<sec){
				if (i_read==512){
					//LED0 = 0;
					sec_read++;
					SD_ReadDisk(buf_read,sec_read,1);
					i_read=0;
					//Update UI
					GUI_UpdateProgbar();
				}
			}
			else if (i_read==i){
				i_read=0;
				sec_read=1;
				SD_ReadDisk(buf_read,sec_read,1);
			}
			PAout(15)=!PAout(15);//�۲����Ƶ��
			//temp=buf_read[i_read++];
			//DAC->DHR8R1=(float)temp/buf_max*200;
			DAC->DHR8R1=buf_read[i_read++];
		}
	}				
	//LCD_ShowxNum(100,250,TIM3->CNT,4,16,0);
	TIM3->SR&=~(1<<0);//����жϱ�־λ 	    
}
//ͨ�ö�ʱ���жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<1;	//TIM3ʱ��ʹ��    
 	TIM3->ARR=arr;  	//�趨�������Զ���װֵ 
	TIM3->PSC=psc;  	//Ԥ��Ƶ������
	TIM3->DIER|=1<<0;   //��������ж�				
	TIM3->CR1|=0x01;    //ʹ�ܶ�ʱ��3
	MY_NVIC_Init(1,3,TIM3_IRQn,2);//��ռ1�������ȼ�3����2									 
}

void TIM4_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef	TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);//����TIM3ʱ�� 

	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;   //��Ƶֵ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;	   //����ģʽ
	TIM_TimeBaseInitStructure.TIM_Period=arr;		   //�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;  //����ʱ�ӷָ�
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);//��������ж�

	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(TIM4,ENABLE);		  //ʹ��TIM3
}

void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)!=RESET)
	{	
		OS_TimeMS++;
	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
}

//������ʱ��6�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��6!
void TIM6_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE); //��ʱ��6ʱ��ʹ��
	
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //���÷�Ƶֵ��10khz�ļ���Ƶ��
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_Period=arr;  //�Զ���װ��ֵ ������5000Ϊ500ms
	TIM_TimeBaseInitStructure.TIM_ClockDivision=0; //ʱ�ӷָ�:TDS=Tck_Tim
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM6,TIM_IT_Update|TIM_IT_Trigger,ENABLE); //ʹ��TIM6�ĸ����ж�

	NVIC_InitStructure.NVIC_IRQChannel=TIM6_IRQn; //TIM6�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1; //��ռ���ȼ�1��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE; //ʹ��ͨ��
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM6,ENABLE); //��ʱ��6ʹ��
}


void TIM6_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM6,TIM_IT_Update)!=RESET)
	{
		GUI_TOUCH_Exec();
	}
	TIM_ClearITPendingBit(TIM6,TIM_IT_Update); //����жϱ�־λ
}




