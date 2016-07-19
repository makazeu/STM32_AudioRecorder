#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "ILI93xx.h"
#include "usart.h"	 
#include "usmart.h" 
#include "24cxx.h"
#include "flash.h"
#include "touch.h"
#include "sram.h"
#include "timer.h"
#include "GUI.h"
#include "GUIDemo.h"
#include "mmc_sd.h"
#include "ui.h"
#include "malloc.h"
#include "dac.h"
#include "adc.h"
/************************************************
 ALIENTEK MiniSTM32������STemWinʵ��
 STemWin ��ֲʵ��
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

int main(void)
{	 			
	Stm32_Clock_Init(9);	//ϵͳʱ������
	uart_init(72,9600);	 	//���ڳ�ʼ��Ϊ9600
	delay_init(72);	    	//��ʱ������ʼ��	  
	LED_Init();			    //LED�˿ڳ�ʼ��
	TFTLCD_Init();			//LCD��ʼ��	
	TP_Init();				//��������ʼ��	
	usmart_dev.init(72);	//��ʼ��USMART		
	mem_init(); 			//��ʼ���ڲ��ڴ��
	KEY_Init();	 			//������ʼ��
	Adc_Init();		  		//ADC��ʼ��
	Dac1_Init();			//DAC��ʼ��
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	TIM4_Int_Init(999,71);	//1KHZ ��ʱ��1ms 
	TIM6_Int_Init(999,719);	//10ms�ж�
	//TIM4_Int_Init(999,719);	//10ms�ж�
	//TIM6_Int_Init(999,7199);
	TIM3_Int_Init(125,71);	//��ʱ��3��ʼ��
	
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC,ENABLE);//ʹ��CRCʱ�ӣ�����STemWin����ʹ�� 
	WM_SetCreateFlags(WM_CF_MEMDEV);
	GUI_Init();
	//GUIDEMO_Graph();
	
	while(SD_Initialize())//��ⲻ��SD��
	{
		LCD_ShowString(60,150,200,16,16, (u8 *)"SD Card Error!");
		delay_ms(500);					
		LCD_ShowString(60,150,200,16,16, (u8 *)"Please Check! ");
		delay_ms(500);
		LED0=!LED0;//DS0��˸
	}
	
	UI_Main();
	//GUIDEMO_Main();
	while(1);
}



