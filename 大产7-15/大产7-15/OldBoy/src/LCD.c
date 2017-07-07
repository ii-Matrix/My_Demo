/***********************************************************
老男孩之                    液晶显示
***********************************************************/
#include "common.h"
#include "include.h"
#include  "LCD.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdarg.h"
#include "string.h"
extern uint8   new_tab ;        //当前选择的变量编号
int16 page,LastPage;             //页数
int16 Img_LCD_Loaction_X=1;      //图像在LCD上的位置
int16 Img_LCD_Loaction_Y=24;
uint8 Flag_LCD_White=0;           //液晶刷白标志位

/****************************
作用：液晶显示总函数
****************************/
void display()
{
  Site_t site;
  int YuZhi = 150;
  if((CarState == CarStop && ImgNum-ImgStopNum==YuZhi))
  {
    LCD_clear(RED);
  }
  if((CarState==CarStop && ImgStopNum == 0) ||(CarState == CarStop && ImgNum-ImgStopNum>YuZhi) || ((CarState == CarStart) && (gpio_get (Boma_6)==1)))
  {
    
    if(page ==0)
    {
      site.x=site.y=0;
 //     LCD_num_BC(site,Time_SD,5, BLUE,RED);
      GiveColor();
      LCD_Camera();
      site.x=64;
      site.y=48;
      LCD_cross(site,48,RED);
      site.y=96;
    //  site.x=0;LCD_FSTR_CH(site,TuXiang ,BLUE, RED);
      site.x=0;if(MiddleError>=0)LCD_char(site,'+', BLUE,RED);else LCD_char(site,'-', BLUE,RED); 
      site.x=8;LCD_num_BC(site,(int)(AbsMiddleError*10),4, BLUE,RED);
      
      site.y=112;
    //  site.x=0;LCD_FSTR_CH(site,DuoJi ,BLUE, RED);
      site.x=0;if(PWM_Duoji>=Dj_Mid)LCD_char(site,'+', BLUE,RED);else LCD_char(site,'-', BLUE,RED); 
      site.x=8;LCD_num_BC(site,abs_sub(PWM_Duoji,Dj_Mid),4, BLUE,RED);
      
      
      site.y=96;
      //site.x=64;LCD_FSTR_CH(site,QianZhan ,BLUE, RED);
      site.x=40;LCD_num_BC(site,Sight,3, BLUE,RED);
      
      site.x = 88;LCD_num_BC(site,SpeedGoal,3, BLUE,RED);
      if(CarType == Front)
      {
        site.x=64;LCD_num_BC(site,RoadType,2, BLUE,RED);
      }
      if(CarType == Later)
      {
        site.x=64;LCD_num_BC(site,RoadType_AnotherCar,2, BLUE,RED);
      }
      site.y=112;
      site.x=40;LCD_char(site,'X',BLUE,RED);
      site.x=48;LCD_num_BC(site,Light_X,3, BLUE,RED);
      site.x=72;LCD_char(site,'Y',BLUE,RED);
      site.x=80;LCD_num_BC(site,Light_Y,3, BLUE,RED);
      site.x=112;LCD_num_BC(site,Distance_TwoCar,3, BLUE,RED);
      //      site.x=104;LCD_num_BC(site,real_y[Sight][80],3, BLUE,RED);
//      
//      site.y=112;
//      site.x=64;LCD_num_BC(site,CheJu,3, BLUE,RED);
    }
    if(page==1)
    {
      site.x=0; site.y=0;   LCD_FSTR_CH(site,GaoSu ,BLUE, RED);
      site.x=0; site.y=16;  LCD_FSTR_CH(site,DiSu ,BLUE, RED);
      site.x=0; site.y=32;  LCD_FSTR_CH(site,DuoJiP_A ,BLUE, RED);
      site.x=0; site.y=48;  LCD_FSTR_CH(site,DuoJiP_B ,BLUE, RED);
      site.x=0; site.y=64;  LCD_FSTR_CH(site,DuoJiP_C ,BLUE, RED);
      site.x=0; site.y=80;  LCD_FSTR_CH(site,DuoJi_D ,BLUE, RED);
      site.x=0; site.y=96;  LCD_FSTR_CH(site,DuoJiFenJieXian ,BLUE, RED);
      site.x=0; site.y=112; LCD_FSTR_CH(site,ShiZi_P,BLUE, RED);
     
      if(LastPage!=1)
      {
        LCD_clear(RED);
        new_tab=0;
      }      
      var_display(1);
    }
    if(page==2)
    {
      site.x=0; site.y=0;   LCD_FSTR_CH(site,ShiZi_D ,BLUE, RED);
      site.x=0; site.y=16;  LCD_FSTR_CH(site,DuoJi_P_XianFu ,BLUE, RED);
      site.x=0; site.y=32;  LCD_FSTR_CH(site,WanQuXiShu ,BLUE, RED);
      site.x=0; site.y=48;  LCD_FSTR_CH(site,DuoJiZhongZhi ,BLUE, RED);
      site.x=0; site.y=64;  LCD_FSTR_CH(site,DuoJiZuoJiXian ,BLUE, RED);
      site.x=0; site.y=80;  LCD_FSTR_CH(site,DuoJiYouJiXian ,BLUE, RED);
      site.x=0; site.y=96;  LCD_FSTR_CH(site,YingYanYuZhi ,BLUE, RED);
      site.x=0; site.y=112; LCD_FSTR_CH(site,DianJi_P ,BLUE, RED);
  
      if(LastPage!=2)
      {
        LCD_clear(RED);
        new_tab=8;
      }
      var_display(2);
    } 
    if(page==3)
    {
      site.x=0; site.y=0;   LCD_FSTR_CH(site,DianJi_I ,BLUE, RED);
      site.x=0; site.y=16;  LCD_FSTR_CH(site,DaiDingCanShu ,BLUE, RED);
      site.x=0; site.y=32;  LCD_FSTR_CH(site,DaiDingCanShu ,BLUE, RED);
      site.x=0; site.y=48;  LCD_FSTR_CH(site,DaiDingCanShu ,BLUE, RED);
      site.x=0; site.y=64;  LCD_FSTR_CH(site,DaiDingCanShu ,BLUE, RED);
      site.x=0; site.y=80;  LCD_FSTR_CH(site,DaiDingCanShu ,BLUE, RED);
      site.x=0; site.y=96;  LCD_FSTR_CH(site,DaiDingCanShu ,BLUE, RED);
      site.x=0; site.y=112; LCD_FSTR_CH(site,DaiDingCanShu ,BLUE, RED);
  
      if(LastPage!=3)
      {
        LCD_clear(RED);
        new_tab=16;
      }
      var_display(3);
    } 
    if(page==4)
    {
 
      site.x=0; site.y=0;  LCD_FSTR_CH(site,MuBiaoSuDu ,BLUE, RED);
      site.x=88;LCD_num_BC(site,SpeedGoal,4, BLUE,RED);
      
      site.x=0; site.y=16;  LCD_FSTR_CH(site,DangQianSuDu ,BLUE, RED);
      if(SpeedNow>=0)
      {
        site.x = 80;LCD_str(site,"+", BLUE,RED);
        site.x = 88;LCD_num_BC(site,SpeedNow,4, BLUE,RED);
      }
      if(SpeedNow<0)
      {
        site.x = 80;LCD_str(site,"-", BLUE,RED);
        site.x = 88;LCD_num_BC(site,-SpeedNow,4, BLUE,RED);
      }
      site.x=0; site.y=32;  LCD_FSTR_CH(site,DianJi_PWM ,BLUE, RED);
      site.x=88;LCD_num_BC(site,PWM_Motor,4, BLUE,RED);
      
      site.x=0; site.y=48;  LCD_FSTR_CH(site,DianYa ,BLUE, RED);
      site.x=88;LCD_num_BC(site,Voltage,4, BLUE,RED);
      
      site.x=0; site.y=64;  LCD_FSTR_CH(site,DuoJi_P ,BLUE, RED);
      site.x=88;LCD_num_BC(site,Duoji_P,4, BLUE,RED);
      
      site.x=0; site.y=80;  LCD_FSTR_CH(site,LuCheng ,BLUE, RED);
      site.x=88;LCD_num_BC(site,Distance,4, BLUE,RED);
      
      site.x=0; site.y=96;  LCD_FSTR_CH(site,DaiDingXianShi ,BLUE, RED);
      site.x=88;LCD_num_BC(site,Dj_Mid,4, BLUE,RED);
      
      site.x=0; site.y=112; LCD_FSTR_CH(site,DaiDingXianShi ,BLUE, RED);
      site.x=88;LCD_num_BC(site,VarReceive[11],4, BLUE,RED);
    }
    if(page == 5)
    {
      Site_t site = {0,0};   
      Size_t size = {128,32};  
      LCD_rectangle(site, size, RED);
      LCD_str(site,"My:", BLUE,RED);
      site.y = 16;
      LCD_str(site,"He:", BLUE,RED);
      site.x = 24;site.y = 0;
      if(RoadType == 0)
        LCD_str(site,"nomal", BLUE,RED);
      else if(RoadType == Cross)
        LCD_str(site,"Cross", BLUE,RED);
      else if(RoadType == CrossL)
        LCD_str(site,"CrossL", BLUE,RED);
      else if(RoadType == CrossR)
        LCD_str(site,"CrossR", BLUE,RED);
      else if(RoadType == StartLine)
        LCD_str(site,"StartLine", BLUE,RED);
      else if(RoadType == GoOutBend)
        LCD_str(site,"GoOutBend", BLUE,RED);
      else if(RoadType == Overtake_Left)
        LCD_str(site,"Overtake_Left", BLUE,RED);  
      else if(RoadType == Overtake_Left_ing)
        LCD_str(site,"Over_Left_ing", BLUE,RED);
      else if(RoadType == Overtake_Right)
        LCD_str(site,"Ov_Right", BLUE,RED);
      else
        LCD_str(site,"other", BLUE,RED);
      site.y = 16;
      if(RoadType_AnotherCar == 0)
        LCD_str(site,"nomal", BLUE,RED);
      if(RoadType_AnotherCar == Overtake_Left)
        LCD_str(site,"Overtake_Left", BLUE,RED);
      site.x = 0;
      site.y = 32;
       if(CurveType == Curve_Null)
        LCD_str(site,"Curve_Null", BLUE,RED);
      if(CurveType == Straight)
        LCD_str(site,"Straight", BLUE,RED);
      else if(CurveType == GoInBend)
        LCD_str(site,"GoInBend", BLUE,RED);  
      else if(CurveType == Bend)
        LCD_str(site,"Bend", BLUE,RED);
      site.y = 48;
      LCD_num_BC(site,StraightNum,4, BLUE,RED);
      
//      site.y = 32;LCD_FSTR_CH(site,CanShu1 ,BLUE, RED);
//      site.y = 48;LCD_FSTR_CH(site,CanShu2 ,BLUE, RED);
//      site.y = 64;LCD_FSTR_CH(site,CanShu3 ,BLUE, RED);
//      site.y = 80;LCD_FSTR_CH(site,ChuanTu ,BLUE, RED);
//      site.y = 96;LCD_FSTR_CH(site,QianChe ,BLUE, RED);
//      site.y = 112;LCD_FSTR_CH(site,GuanPingMu ,BLUE, RED);
    }
  }
}
/****************************
作用：在液晶上显示图像
****************************/
void LCD_Camera()
{
  uint8 x,y;
  Site_t site;
  Size_t size;
  site.x=site.y=0;
  size.W=128;size.H=96;
  LCD_PTLON(site, size);             //开窗
  LCD_RAMWR();                       //写内存 
  
  for(y=Img_LCD_Loaction_Y;y<96+Img_LCD_Loaction_Y;y++)
  {
    for(x=Img_LCD_Loaction_X*16;x<128+Img_LCD_Loaction_X*16;x++)
    {              
      if(Img[y][x]==ImgWhite)
        LCD_WR_DATA(WHITE);
      
      if(Img[y][x]==ImgBlack)
          LCD_WR_DATA(BLACK);
      if(Img[y][x]==ImgGreen)
        LCD_WR_DATA(GREEN);
      if(Img[y][x]==ImgBlue)
        LCD_WR_DATA(BLUE);             
      if(Img[y][x]==ImgRed)
        LCD_WR_DATA(RED);         
    }
  }
  if(CarType == Later) 
  {
    site.x = Light_X - Img_LCD_Loaction_X*16;
    site.y = Light_Y - Img_LCD_Loaction_Y;
    if(site.x<118 && site.x>10 && site.y<96 && site.y>10)
      LCD_cross(site,10,RED);
  }
  if(CarType == Front) 
  {
    site.x =  RoadScan.pRes.x - Img_LCD_Loaction_X*16;
    site.y =  RoadScan.pRes.y - Img_LCD_Loaction_Y;
    if(site.x<118 && site.x>10 && site.y<96 && site.y>10)
    {
      if(RoadScan.pRes.state == Tri_Left)
        LCD_cross(site,10,RED);
      if(RoadScan.pRes.state == Tri_Right)
        LCD_cross(site,10,BLUE);
    }
  }
}
/*********************************************
作用：改变数组相应位置的值，为液晶显示做准备
*********************************************/
void GiveColor()
{
  for(int y=119;y>=Sight_L;y--)
    Img[y][Left[y]]=ImgBlue;
  for(int y=119;y>=Sight_R;y--)
    Img[y][Right[y]]=ImgRed;
  for(int y=119;y>=Sight;y--)
  {
  //  if(SelectRowFlag[y] == 1)
    {
//             if((Lost_L[y]==LostWhite && Lost_R[y]==LostWhite) || (Lost_L[y]==LostBlack && Lost_R[y]==LostBlack)||(Middle[y]==Left_Init) || (Middle[y]==Right_Init))
      if((Lost_L[y]==LostWhite && Lost_R[y]==LostWhite) || (Lost_L[y]==LostBlack && Lost_R[y]==LostBlack))
      {//本行不参与偏差计算
      }
      else
        Img[y][Middle[y]]=ImgGreen;   
  
    }
  }
}

/*********************************************
作用：字符串显示
参数：起始横坐标，纵坐标，字符串
*********************************************/
void LCD_MyStr(uint16 x,uint16 y,uint8 *Str)
{
#define MAX_CHAR_POSX (LCD_W-8)
#define MAX_CHAR_POSY (LCD_H-16)
  Site_t site;
  site.x=x;
  site.y=y;
  
  while(*Str != '\0')
  {
    if(site.x > MAX_CHAR_POSX )
    {
      //换行
      site.x = 0;
      site.y += 16;
    }
    if(site.y > MAX_CHAR_POSY )
    {
      //一屏
      site.y = 0;
      site.x = 0;
    }
    
    LCD_char(site, *Str, BLUE, RED);
    site.x += 8;
    Str ++ ;
  }
#undef MAX_CHAR_POSX
#undef MAX_CHAR_POSY
}
/*********************************************
作用：数字显示
参数：起始横坐标，纵坐标，数字，数字长度
*********************************************/
void LCD_MyNum(uint16 x,uint16 y, uint32 num, uint8 max_num_bit )
{      
  Site_t site;
  site.x=x;
  site.y=y;
  uint32 res = num;
  uint8 t = 0;
  Site_t sitetemp;
  Size_t size;
  sitetemp.y = site.y; 
  if( num == 0 )
  {
    LCD_char(site, '0',BLUE, WHITE);  
    site.x += 8;
    size.H  = 16;
    size.W  = 8 * (max_num_bit - 1);
    LCD_rectangle(site, size,WHITE);  
    return;
  }
  while( res )            //得到数字长度t
  {
    res /= 10;
    t++;
  }
  if(t >= max_num_bit )    //限制最大长度
  {
    t = max_num_bit;
  }
  res = t;
  while( t != 0 )
  {
    sitetemp.x = site.x + (8 * (t--) - 8);
    LCD_char(sitetemp, (num % 10) + '0', BLUE, WHITE);
    num /= 10 ;
  }
  if(res != max_num_bit )
  {
    size.W = 8 * (max_num_bit - res);
    site.x += (8 * res);
    size.H  = 16;
    LCD_rectangle(site, size, WHITE);
  }
}
void LCD_Printf(uint16 x, uint16 y, char* format, ...)
{
  unsigned char s[128];
  va_list arg_ptr;
  Site_t site = {x,y};
  va_start(arg_ptr, format);
  vsnprintf(s, sizeof(s)-1, format, arg_ptr);
  LCD_str(site, s, BLUE, RED);
  va_end(arg_ptr);
}