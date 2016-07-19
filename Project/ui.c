#include "GUIDEMO.h"
#include "FramewinDLG.h"
#include "led.h"
#include "malloc.h"
#include "function.h"
#include "mmc_sd.h"
#include "dac.h"
#include "adc.h"
#include "stdio.h"
#include "ui.h"
#include "timer.h"
#include "delay.h"

u8 state=0;
extern u8 tim3_en;

u16 adcx;
u16 temp=0;	 
u8 *buf;
u8 buf_max;
u8 *buf_read;
u16 i=0,sec=0;
u16 i_read=0,sec_read=0;
char printedStr2[32]="Sec: ";

char* GetSec(){
	sprintf(printedStr2 + 5,"%u",sec);
	return printedStr2;
}

void status_toAdc(){
	state = 1;
}

void status_toDac(){
	state = 2;
	LED0 = 1;
	sec_read = 1;
	SD_ReadDisk(buf_read,sec_read,1);
	i_read = 0;
}

void touch_stop(){
	if(state == 1){
		SD_WriteDisk(buf,sec,1);
		save_mem(sec,i,buf_max);
	}
	else if (state == 2){
		DAC->DHR8R1=0;//DA输出0
	}
	state = 0;
	LED0 = 1;
}

void SD_Read_Sectorx(u32 sec)
{
	u8 *buf;
	u16 i;
	buf=mymalloc(512);				//申请内存
	if(SD_ReadDisk(buf,sec,1)==0)	//读取0扇区的内容
	{	
		//LCD_ShowString(60,230,200,16,16,"USART1 Sending Data...");
		printf("SECTOR %d DATA:\r\n",sec);
		for(i=0;i<512;i++){
			printf("%x ",buf[i]);//打印sec扇区数据
			while (!tim3_en);
			DAC->DHR8R1=buf[i];
		}			
		printf("\r\nDATA ENDED\r\n");
		//LCD_ShowString(60,230,200,16,16,"USART1 Send Data Over!");
	}
	myfree(buf);//释放内存	
}

void UI_Main(){
	LED0 = 1;
	PAout(15) = 0;
	buf=mymalloc(512);
	buf_read=mymalloc(512);
	load_mem(&sec,&i,&buf_max);//读取上次记录数据
	DAC->DHR8R1=0;
	SD_WriteDisk(buf,sec,1);
	
	BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
	CreateFramewin();
	while(1)
	{
		/*
		switch (state){
			case 0:
				GUI_Delay(1);
				break;
			case 1:
				while(!tim3_en);//等待允许信号
				
				tim3_en=0;
				PAout(15)=!PAout(15);//示波器观看
				//adcx=Get_Adc_Average(ADC_CH1,10);		
				adcx=Get_Adc(ADC_CH1);
				buf[i]=adcx>>4;
				if (buf[i]>buf_max) buf_max=buf[i];
				i++;
				//GUI_Exec();
			    LED0 = 0;
			    GUI_Delay(1);
				//printf("state: %d\r\n", state);
				if (i==512){
					SD_WriteDisk(buf,sec,1);
					sec++;
					i=0;
				}
				//LED0=!LED0;//观察采样频率
				break;
				GUI_Delay(1);
				break;
			case 2:
				while(!tim3_en);
				tim3_en=0;
				PAout(15)=!PAout(15);//观察采样频率
				temp=buf_read[i_read++];
				DAC->DHR8R1=(float)temp/buf_max*200;
			    GUI_Delay(1);
				break;
			default:
				break;
		}*/
		GUI_Delay(1);
	}

}

void load_mem(u16 *sec,u16 *i,u8 *max){
	u8 *buf_mem;
	buf_mem=mymalloc(512);
	SD_ReadDisk(buf_mem,0,1);
	*sec=buf_mem[0];
	*sec<<=8;
	*sec+=buf_mem[1];
	*i=buf_mem[2];
	*i<<=8;
	*i+=buf_mem[3];
	*max=buf_mem[4];
	myfree(buf_mem);
}
void save_mem(u16 sec,u16 i,u8 max){
	u8 *buf_mem;
	buf_mem=mymalloc(512);
	buf_mem[0]=sec>>8;
	buf_mem[1]=sec;
	buf_mem[2]=i>>8;
	buf_mem[3]=i;
	buf_mem[3]=max;
	SD_WriteDisk(buf_mem,0,1);
	myfree(buf_mem);
}




