#ifndef _OLDBOY_LCD_H_
#define _OLDBOY_LCD_H_
void display();         //Һ����ʾ�ܺ���
void LCD_Camera();      //��Һ������ʾͼ��
void LCD_MyStr(uint16 x,uint16 y,uint8 *Str);//Һ������ʾ�ַ���
void LCD_MyNum(uint16 x,uint16 y, uint32 num, uint8 max_num_bit );//Һ������ʾ����
void LCD_Printf(uint16 x, uint16 y, char* format, ...);
void GiveColor();
#endif 