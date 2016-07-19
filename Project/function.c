#include "sys.h"
#include "mmc_sd.h"
#include "dac.h"
#include "adc.h"
#include "led.h"
#include "stdio.h"
#include "malloc.h"
#include "function.h"


char printedStr[32]="SD: ";


char* GetSDSectorCount(){
	u32 sd_size;
	sd_size=SD_GetSectorCount();//得到扇区数
	sprintf(printedStr + 4,"%u",sd_size>>11);
	return printedStr;
}

void ledtest(){
	LED0 = !LED0;
}


