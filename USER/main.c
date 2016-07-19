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
 ALIENTEK MiniSTM32开发板STemWin实验
 STemWin 移植实验
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

int main(void)
{	 			
	Stm32_Clock_Init(9);	//系统时钟设置
	uart_init(72,9600);	 	//串口初始化为9600
	delay_init(72);	    	//延时函数初始化	  
	LED_Init();			    //LED端口初始化
	TFTLCD_Init();			//LCD初始化	
	TP_Init();				//触摸屏初始化	
	usmart_dev.init(72);	//初始化USMART		
	mem_init(); 			//初始化内部内存池
	KEY_Init();	 			//按键初始化
	Adc_Init();		  		//ADC初始化
	Dac1_Init();			//DAC初始化
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	TIM4_Int_Init(999,71);	//1KHZ 定时器1ms 
	TIM6_Int_Init(999,719);	//10ms中断
	//TIM4_Int_Init(999,719);	//10ms中断
	//TIM6_Int_Init(999,7199);
	TIM3_Int_Init(125,71);	//定时器3初始化
	
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC,ENABLE);//使能CRC时钟，否则STemWin不能使用 
	WM_SetCreateFlags(WM_CF_MEMDEV);
	GUI_Init();
	//GUIDEMO_Graph();
	
	while(SD_Initialize())//检测不到SD卡
	{
		LCD_ShowString(60,150,200,16,16, (u8 *)"SD Card Error!");
		delay_ms(500);					
		LCD_ShowString(60,150,200,16,16, (u8 *)"Please Check! ");
		delay_ms(500);
		LED0=!LED0;//DS0闪烁
	}
	
	UI_Main();
	//GUIDEMO_Main();
	while(1);
}



