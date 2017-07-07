/***********************************************************
老男孩之               SD卡存储及读取
***********************************************************/
#include "common.h"
#include "include.h"
#include  "SD.h"
 FATFS fs;    //文件系统
 FIL   fdst;   //文件
uint8 SD_Var[(VarReceiveMax+VarSendMax)*3];
uint8 SD_Boundary[CAMERA_H*2*3];//由于上位机用java写，java中byte类型范围为-128~127，而分辨率为160，超过了127，所以要转换一下。

/****************************
作用：SD卡初始化
****************************/
void  SD_Init(void)
{
  int     res;
  char    myfilename[200];
  uint32  Imag_num = 1;
  //  memset(buff,0,sd_buff_size);      //对数组清零
  //   sd_fenshu=4;
  f_mount(0, &fs);
  do
  {
    Imag_num ++;
    sprintf(myfilename, "0:/%d.txt",Imag_num);
    res = f_open( &fdst , myfilename, FA_CREATE_NEW |FA_OPEN_ALWAYS | FA_WRITE | FA_READ);  //打开文件，如果没有就创建，带读写打开
    
    if(fs.fs_type == 0)
    {
      fdst.fs = 0;
      return;
    }
    
  }while(res == FR_EXIST);        //如果文件存在，则命名继续加1
  
  if ( res == FR_OK )
  {
      printf( "\n文件打开成功 \n" );  
    //res = f_write(&fdst, imgsize, sizeof(imgsize),&mybw);  //先写入高和宽，方便上位机处理
  }
  else
  {
    f_close(&fdst);
    fdst.fs = 0;
  }
}
/****************************
作用：SD卡存储
****************************/  
void SD_Save()
{
    pit_time_start  (PIT2); 
    uint32 mybw;
    uint8 *SD_Boundry_Start = ImgAgo+CAMERA_SIZE;  
    for(uint8 i = 0;i<120;i++)                  //拷贝边界数组
    {
      *(SD_Boundry_Start+i)   = Left[i];
      *(SD_Boundry_Start+120+i) = Middle[i];
      *(SD_Boundry_Start+240+i) = Right[i];
    }
    VarToSD();    
    if(CarState==CarStart)             //车跑起来时进行存储而不保存，保存太耗时
    {
      f_write(&fdst,(uint8*)ImgAgo,2860 ,&mybw);  //写
     // f_write(&fdst,(uint8*)ImgAgo+2400,460 ,&mybw);  //舍弃背景色，节省写入时间
    }
   if(CarState==CarStop)              //车停时进行文本的保存，此时才算真的写入sd卡
    {
      f_sync(&fdst);
    }
    Time_SD = pit_time_get_us    (PIT2);
    
}
/****************************
作用：变量转换为SD卡存储格式
****************************/  
void VarToSD()
{
  uint8 *SD_Var_Start = ImgAgo+CAMERA_SIZE+360;
  for(uint8 i=0;i<25;i++)
  {
    *(SD_Var_Start+i*2)   = (uint8)((VarReceive[i]&0x0000ffff)>>8);
    *(SD_Var_Start+i*2+1) = (uint8)(VarReceive[i]&0x0000ffff);
  }
  for(int i = 0;i<25;i++)
  {
      *(SD_Var_Start+50+i*2) = (uint8)((VarSend[i]&0x0000ffff)>>8);  
      *(SD_Var_Start+50+i*2+1) = (uint8)((VarSend[i]&0x0000ffff));     
  }
}