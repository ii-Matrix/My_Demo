#ifndef _OLDBOY_LCD_H_
#define _OLDBOY_LCD_H_
void display();         //液晶显示总函数
void LCD_Camera();      //在液晶上显示图像
void LCD_MyStr(uint16 x,uint16 y,uint8 *Str);//液晶上显示字符串
void LCD_MyNum(uint16 x,uint16 y, uint32 num, uint8 max_num_bit );//液晶上显示数字
void LCD_Printf(uint16 x, uint16 y, char* format, ...);
void GiveColor();
#endif 