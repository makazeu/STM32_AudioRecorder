#include "GUI.h"
#include "sram.h"
#include "malloc.h"

//����EMWIN�ڴ��С
#define GUI_NUMBYTES  (30*1024)
#define GUI_BLOCKSIZE 0X80  //���С

//GUI_X_Config
//��ʼ����ʱ�����,��������emwin��ʹ�õ��ڴ�
void GUI_X_Config(void) {

	U32 *aMemory = mymalloc(GUI_NUMBYTES); //���ڲ�RAM�з���GUI_NUMBYTES�ֽڵ��ڴ�
	GUI_ALLOC_AssignMemory((U32 *)aMemory, GUI_NUMBYTES); //Ϊ�洢����ϵͳ����һ���洢��
	GUI_ALLOC_SetAvBlockSize(GUI_BLOCKSIZE); //���ô洢���ƽ���ߴ�,����Խ��,���õĴ洢������Խ��
	GUI_SetDefaultFont(GUI_FONT_6X8); //����Ĭ������
}
