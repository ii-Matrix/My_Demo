/*******************************************************************
���к�֮                 ������
*******************************************************************/
#include "common.h"
#include "include.h"
extern RoadScanType RoadScan;   //��ֲʱ������������

void Run();

void  main(void)
{
  InitAll();                          //ȫ���ĳ�ʼ��
  while(1)
  {
    pit_time_start(PIT0);         //��ִ��ʱ�䣬��Ҫ�ǿ���ʱ���Ƿ����ң�һ������ʱ�䳬���ɼ�ʱ�䣬���ٻᶪʧһ��ͼ��
    ov7725_eagle_get_img();         //����ͷ��ȡͼ��
    Time_Camera = pit_time_get_us(PIT0);
  }
}
/*************************
���ã��������к������ܺ������ڳ��ж������
*************************/
void Run()
{

  img_extract((uint8*)Img,ImgAgo,CAMERA_SIZE);  //��ѹ��,һά������ά����
  StartLineCheck();                             //�����߼��
 // LCD_Find();                                   //��ԴѰ��

  BottomFind();                                 //�ײ�Ѱ��
  GenZong();                                    //���ٱ�Ե  
  ForeSight();                                  //ȷ��ǰհ
  jisuan_Middle();                              //��������
//  Middle_Correct();                              //ǰ���ͱ߽����һ��  
//  Middle_Correct2();                             //ǰ���ͱ߽����һ��
  
  //RodRemove();                                 //ȥ��ǰ������ͷ֧�ܵ�Ӱ��
  
  judge_road();                                 //�ж���������         
  MiddleError=jisuan_piancha();                 //����ƫ��
  AbsMiddleError=Myabsfloat(MiddleError); 
  
  Duoji();                                      //������PWM�����
  SpeedSet();                                   //�����趨�ٶ�
  Exchange_Var();                                //��������
  Time_Suanfa = pit_time_get_us(PIT0);           //����ʱ�� 
  display();                                     //Һ����ʾ
  
  
  Send_Var();                                    //���ͱ���                                                             
  
 // SD_Save();                                     //sd���洢
  deal_key_event();                              //������                              
  
  SetZero();                                     //���������������ֵ                                       
// if(CarState == CarStart)
  ImgNum++;
}



