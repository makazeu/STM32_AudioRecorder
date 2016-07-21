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
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK 战舰开发板
//定时器 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/1/13
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
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

//定时器3中断服务程序	 
void TIM3_IRQHandler(void)
{ 
	//LCD_ShowxNum(60,250,TIM3->CNT,4,16,0);
	if(TIM3->SR&0X0001)//溢出中断
	{
		//LED1=!LED1;
		//PAout(15)=!PAout(15);//示波器观看
		//if (state) tim3_en=1;
		//else tim3_en=0;
		if (state == 1) {
			PAout(15)=!PAout(15);//示波器观看
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
			PAout(15)=!PAout(15);//观察采样频率
			//temp=buf_read[i_read++];
			//DAC->DHR8R1=(float)temp/buf_max*200;
			DAC->DHR8R1=buf_read[i_read++];
		}
	}				
	//LCD_ShowxNum(100,250,TIM3->CNT,4,16,0);
	TIM3->SR&=~(1<<0);//清除中断标志位 	    
}
//通用定时器中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<1;	//TIM3时钟使能    
 	TIM3->ARR=arr;  	//设定计数器自动重装值 
	TIM3->PSC=psc;  	//预分频器设置
	TIM3->DIER|=1<<0;   //允许更新中断				
	TIM3->CR1|=0x01;    //使能定时器3
	MY_NVIC_Init(1,3,TIM3_IRQn,2);//抢占1，子优先级3，组2									 
}

void TIM4_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef	TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);//开启TIM3时钟 

	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;   //分频值
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;	   //计数模式
	TIM_TimeBaseInitStructure.TIM_Period=arr;		   //自动重装数值
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;  //设置时钟分割
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);//允许更新中断

	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(TIM4,ENABLE);		  //使能TIM3
}

void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)!=RESET)
	{	
		OS_TimeMS++;
	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
}

//基本定时器6中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器6!
void TIM6_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE); //定时器6时钟使能
	
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //设置分频值，10khz的计数频率
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_Period=arr;  //自动重装载值 计数到5000为500ms
	TIM_TimeBaseInitStructure.TIM_ClockDivision=0; //时钟分割:TDS=Tck_Tim
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM6,TIM_IT_Update|TIM_IT_Trigger,ENABLE); //使能TIM6的更新中断

	NVIC_InitStructure.NVIC_IRQChannel=TIM6_IRQn; //TIM6中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1; //先占优先级1级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE; //使能通道
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM6,ENABLE); //定时器6使能
}


void TIM6_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM6,TIM_IT_Update)!=RESET)
	{
		GUI_TOUCH_Exec();
	}
	TIM_ClearITPendingBit(TIM6,TIM_IT_Update); //清除中断标志位
}




