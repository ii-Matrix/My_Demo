/***********************************************************
���к�֮               SD���洢����ȡ
***********************************************************/
#include "common.h"
#include "include.h"
#include  "SD.h"
 FATFS fs;    //�ļ�ϵͳ
 FIL   fdst;   //�ļ�
uint8 SD_Var[(VarReceiveMax+VarSendMax)*3];
uint8 SD_Boundary[CAMERA_H*2*3];//������λ����javaд��java��byte���ͷ�ΧΪ-128~127�����ֱ���Ϊ160��������127������Ҫת��һ�¡�

/****************************
���ã�SD����ʼ��
****************************/
void  SD_Init(void)
{
  int     res;
  char    myfilename[200];
  uint32  Imag_num = 1;
  //  memset(buff,0,sd_buff_size);      //����������
  //   sd_fenshu=4;
  f_mount(0, &fs);
  do
  {
    Imag_num ++;
    sprintf(myfilename, "0:/%d.txt",Imag_num);
    res = f_open( &fdst , myfilename, FA_CREATE_NEW |FA_OPEN_ALWAYS | FA_WRITE | FA_READ);  //���ļ������û�оʹ���������д��
    
    if(fs.fs_type == 0)
    {
      fdst.fs = 0;
      return;
    }
    
  }while(res == FR_EXIST);        //����ļ����ڣ�������������1
  
  if ( res == FR_OK )
  {
      printf( "\n�ļ��򿪳ɹ� \n" );  
    //res = f_write(&fdst, imgsize, sizeof(imgsize),&mybw);  //��д��ߺͿ�������λ������
  }
  else
  {
    f_close(&fdst);
    fdst.fs = 0;
  }
}
/****************************
���ã�SD���洢
****************************/  
void SD_Save()
{
    pit_time_start  (PIT2); 
    uint32 mybw;
    uint8 *SD_Boundry_Start = ImgAgo+CAMERA_SIZE;  
    for(uint8 i = 0;i<120;i++)                  //�����߽�����
    {
      *(SD_Boundry_Start+i)   = Left[i];
      *(SD_Boundry_Start+120+i) = Middle[i];
      *(SD_Boundry_Start+240+i) = Right[i];
    }
    VarToSD();    
    if(CarState==CarStart)             //��������ʱ���д洢�������棬����̫��ʱ
    {
      f_write(&fdst,(uint8*)ImgAgo,2860 ,&mybw);  //д
     // f_write(&fdst,(uint8*)ImgAgo+2400,460 ,&mybw);  //��������ɫ����ʡд��ʱ��
    }
   if(CarState==CarStop)              //��ͣʱ�����ı��ı��棬��ʱ�������д��sd��
    {
      f_sync(&fdst);
    }
    Time_SD = pit_time_get_us    (PIT2);
    
}
/****************************
���ã�����ת��ΪSD���洢��ʽ
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