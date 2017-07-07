/*******************************************************************
老男孩之                 主函数
*******************************************************************/
#include "common.h"
#include "include.h"
extern RoadScanType RoadScan;   //移植时加入主函数中

void Run();

void  main(void)
{
  InitAll();                          //全部的初始化
  while(1)
  {
    pit_time_start(PIT0);         //测执行时间，主要是看看时间是否紊乱，一旦运算时间超过采集时间，至少会丢失一幅图像
    ov7725_eagle_get_img();         //摄像头获取图像
    Time_Camera = pit_time_get_us(PIT0);
  }
}
/*************************
作用：几乎所有函数的总函数，在场中断里调用
*************************/
void Run()
{

  img_extract((uint8*)Img,ImgAgo,CAMERA_SIZE);  //解压缩,一维数组变二维数组
  StartLineCheck();                             //起跑线检测
 // LCD_Find();                                   //光源寻找

  BottomFind();                                 //底部寻找
  GenZong();                                    //跟踪边缘  
  ForeSight();                                  //确定前瞻
  jisuan_Middle();                              //计算中线
//  Middle_Correct();                              //前车和边界混在一起  
//  Middle_Correct2();                             //前车和边界混在一起
  
  //RodRemove();                                 //去除前车摄像头支架的影响
  
  judge_road();                                 //判断赛道类型         
  MiddleError=jisuan_piancha();                 //计算偏差
  AbsMiddleError=Myabsfloat(MiddleError); 
  
  Duoji();                                      //计算舵机PWM并输出
  SpeedSet();                                   //更改设定速度
  Exchange_Var();                                //交换参数
  Time_Suanfa = pit_time_get_us(PIT0);           //运算时间 
  display();                                     //液晶显示
  
  
  Send_Var();                                    //发送变量                                                             
  
 // SD_Save();                                     //sd卡存储
  deal_key_event();                              //处理按键                              
  
  SetZero();                                     //参数清零或其他初值                                       
// if(CarState == CarStart)
  ImgNum++;
}



