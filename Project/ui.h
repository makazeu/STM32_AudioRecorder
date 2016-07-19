#include "sys.h"

void UI_Main(void);
void SD_Read_Sectorx(u32 sec);
void save_mem(u16 sec,u16 i,u8 max);
void load_mem(u16 *sec,u16 *i,u8 *max);
void status_toAdc(void);
void status_toDac(void);
void touch_stop(void);
char* GetSec(void);
