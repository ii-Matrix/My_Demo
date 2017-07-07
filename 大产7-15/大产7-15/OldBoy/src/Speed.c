/***********************************************************
���к�֮               �ٶȿ���
***********************************************************/
#include "common.h"
#include "include.h"
#include  "Speed.h"
#include "Var.h"
int SpeedMax;           //����ٶ�
int SpeedMid;           //�м��ٶ�
float SpeedChase_Range;   //׷���ٶȱ仯��Χ
int SpeedChase_KP;      //׷���ٶȱ仯ϵ��
int SpeedTriangle;      //���ǳ��������ٶ�
int SpeedMin;                   //��С�ٶ�
int SpeedGoal;                  //Ŀ���ٶ�
int SpeedGoal_AnotherCar;       //��һ�����ٶ�
int Speed_P;                    //�ٶ�P��
int Speed_I;                    //�ٶ�I��
int Motor_P;                    //���P����
int Motor_I;                    //���I����
int MotorOutMax;                //���������PWM
int MotorOutMin=0;              //�����С���PWM
int16 SpeedNow;                 //��ǰ�ٶ�
int PWM_BangBangMax;            //bangbang�������
int PWM_BangBangMin;            //bangbang�������
int BangBang_Yuzhi_Up;             //bangbang��ֵ
int BangBang_YuZhi_Down;
int Flag_BangBang;              //bangbang��־λ
int MotorError[3];              //���ƫ��
int PWM_Motor;                  //���PWM
int CarTime;                    //��������һȦ��ʱ��
int SpeedAverage;               //һȦ������ƽ���ٶ�
int PID_Test_Time;              //�ٶ�PID���β�����ʱ��
float Distance,Distance_Last;                 //·�̣���λ������
float Distance_StartLine;       //������·�̼�¼
float Distance_Overtake;
float Distance_GoOutBend;       //����·�̼�¼
float Distance_GoInBend;
float Distance_Cross,Distance_Cross_GoOut;
float Distance_Overtake_Right_GoStraight;
float Distance_Overtake_Left_GoStraight;
extern uint8 Sonic_AnotherState;
long Count_Total,Count_Total_Last;               //�����ۼ�ֵ
int Distance_TwoCar;          //˫������
uint8 Flag_Brake;               //ɲ����־λ
Speed_Flag SpeedFlag,SpeedFlag_Last;           //�ٶȿ��Ʊ�־λ
int ReversalPower_Slight = -700;              //��ת����
int ReversalPower_Urgency = -2000;
int PWM_Overtake;
float Kv;
//float Light_Far  = 71.0;
//float Light_Near = 91.0;
//float Light_Close = 91.0;
//float Light_VeryFar = 75.0; 
//float Light_Mid = 81.0;
float Distance_Far  = 12.0;
float Distance_Near = 5.0;
float Distance_Close = 5.0;
float Distance_VeryFar = 12.0; 
float Distance_Mid = 8.0;

/****************************
δ��֤
���ã��ٶȿ���
****************************/
void SpeedSet()
{

  if(CarType == Front)
  {
    int YuZhi_Sight   = 33;
    int YuZhi_Piancha = 14;
    if(CurveType == Straight)
      SpeedGoal = SpeedMax;
    else if ( CurveType = Bend )
      SpeedGoal = SpeedMin;
    else
      SpeedGoal = SpeedMid;
//    else
//      SpeedGoal = SpeedMax - Kv/100.0*AbsMiddleError;
     if(SpeedGoal<SpeedMin)
       SpeedGoal = SpeedMin;

//    if(Distance_TwoCar>Distance_VeryFar)
//      SpeedGoal = SpeedMid;
//    if(RoadType == Overtake_Left_ing || RoadType == Overtake_Right_ing)
//    {
//      if(RoadType_Last == Overtake_Left || RoadType == Overtake_Right)
//      {
//        SpeedGoal = 0;
//        SpeedFlag = Brake;
//        PWM_Motor = 0;
//        MotorOut(PWM_Motor); 
//      }
//      else                //�����ƶ�
//      {
//        SpeedGoal = 0;
//        if(SpeedNow>3)
//        {
//          SpeedFlag = Reversal_Slight; 
//          PWM_Motor = PWM_Overtake;
//          ftm_pwm_duty(FTM0, MotorCH2,PWM_Motor);
//          ftm_pwm_duty(FTM0, MotorCH1,0);
//        }
//        else
//        {
//          SpeedGoal = 0;
//          SpeedFlag = Brake;
//          PWM_Motor = 0;
//          MotorOut(PWM_Motor); 
//        }
//      }
//    } 
//    if(RoadType == Overtake_Right_Rerturn_Prepare)
//    {
//      SpeedGoal = 0;
//      SpeedFlag = Reversal_Urgency;    
//    }
//    if(RoadType == Overtake_Right_Rerturn_Execute)
//    {
//      SpeedGoal = SpeedMax;
//      SpeedFlag = Speed_Null;    
//    }
//
  }

}
/****************************
���ã�������PWM
����������õ�PWM
****************************/
void MotorOut(int PWMOut)
{
  if(CarState == CarStart && Distance<200 && SpeedNow<SpeedGoal*0.96)//�����һ�ף�����޷�4000��Ϊ�˸����������
  {
    if(SpeedFlag!= Brake)
  //    PWMOut = MotorOutMax = 4000*SpeedGoal/70.0*8000.0/Voltage;
    PWMOut = MotorOutMax = 4000*SpeedGoal/70.0;
  }
  else
   // MotorOutMax = 3700*8000.0/Voltage;
    MotorOutMax = 3000;
  
  if(SpeedFlag == Reversal_Slight)
  {
    if(PWMOut > MotorOutMax)             //�޷�      
      PWMOut = MotorOutMax ;
    else if(PWMOut < ReversalPower_Slight)
      PWMOut = ReversalPower_Slight ;    
  }
  else if(SpeedFlag == Reversal_Urgency)
  {
    if(PWMOut > MotorOutMax)             //�޷�      
      PWMOut = MotorOutMax ;
    else if(PWMOut < ReversalPower_Urgency)
      PWMOut = ReversalPower_Urgency ;    
  }
  else
  {
    if(PWMOut > MotorOutMax)             //�޷�      
      PWMOut = MotorOutMax ;
    else if(PWMOut < MotorOutMin)
      PWMOut = MotorOutMin ;
  }
  if(PWMOut >= 0)
  {
    ftm_pwm_duty(FTM0, MotorCH2,PWMOut);
    ftm_pwm_duty(FTM0, MotorCH1,0);
  }
  else
  {
    ftm_pwm_duty(FTM0, MotorCH1,-PWMOut);
    ftm_pwm_duty(FTM0, MotorCH2,0);
  }

}
/****************************
���ã�����ʽPID
****************************/
void SpeedPID()    
{
  MotorError[2]=MotorError[1];
  MotorError[1]=MotorError[0];
  MotorError[0]=SpeedGoal-SpeedNow;
  Speed_P = Motor_P*(MotorError[0]-MotorError[1]);
  Speed_I = Motor_I*(MotorError[0]);
//  if(Speed_I>200)               //�����޷�
//  {
//    Speed_I = 200;
//  }
//  else if (Speed_I <-200)
//  {
//    Speed_I = -200;
//  }
  if(abs(MotorError[0])>1)             //��������
    PWM_Motor += (Speed_P + Speed_I);
 // PWM_Motor = PWM_Motor*8000.0/Voltage;//��ѹУ��

//  if(PWM_Motor>MotorOutMax*8000.0/Voltage)      
//    PWM_Motor=MotorOutMax*8000.0/Voltage;
    if(PWM_Motor>MotorOutMax)      
    PWM_Motor=MotorOutMax;
  if(PWM_Motor<MotorOutMin)
    PWM_Motor=MotorOutMin;
}
/****************************
���ã�·�̼�¼
****************************/
void DistanceRecord()
{
  Count_Total+=SpeedNow;
  Distance = (float)Count_Total/67.0;
}
/****************************
���ã�bangbang����
****************************/
void BangBang()
{
  int Error = SpeedGoal - SpeedNow; 
  BangBang_Yuzhi_Up = 10; 
  BangBang_YuZhi_Down = -7;
  if(Error>0 && Sight - Sight_Init<7)            // ������
  {
  //  BangBang_Yuzhi=SpeedGoal*0.8;
    if(Error>BangBang_Yuzhi_Up)
    {
      SpeedFlag=Speed_Up;
      PWM_Motor = MotorOutMax;
    }
    else
    {
      SpeedFlag=Speed_Null;
    }
    
  }
//  if(SpeedGoal == SpeedMin)
//  {
//    if(Error<BangBang_YuZhi_Down)
//    {
//      SpeedFlag = Brake;
//      PWM_Motor = 0;
//    }
//    else
//    {
//      SpeedFlag=Speed_Null;
//    }
//  }
  if(Error<= 0)
  {
    SpeedFlag=Speed_Null;
  }
//  else if(Error<0 && abs(Error)>BangBang_Yuzhi)//  ������
//  {
//    PWM_Motor = MotorOutMax;
//  }
}
/****************************
���ã������������
****************************/
void KaiHuan()
{
  if(RoadType == Straight)     
  {
    PWM_Motor = SpeedMax;
  }
  else if(RoadType == Bend)
  {
    PWM_Motor =  SpeedMin+BendNum*2;//����𽥼���
  }
  else  //�ٶȶ������ߣ��ٶ��趨�����ĺ��ģ�Ȼ���Ҳ����ᡣ����
        //��ſ��˿��������棬�����ַ�����һ���Ǻ���Զǰհ�йأ�һ����ͼ��ƫ���йأ�����Ҫ��MATLAB��EXCEL��϶�������
        //Ȼ�󣬸��ݲ�ͬ����������̬�ı����й�ʽ�е��ٶ����ֵ����Сֵ
  {
  //  PWM_Motor = SpeedMax - (SpeedMax-SpeedMin)
  }
}
/******************
���ã��ٶ�PID���β�����
��������
���أ���
******************/
void PID_Test()
{
  PID_Test_Time++;
  if(PID_Test_Time==100)
  {
    SpeedGoal = SpeedMax;
  }
  if(PID_Test_Time==200)
  {
    PID_Test_Time=0;
    SpeedGoal = SpeedMin;
  }
}
/******************
���ã��ٶ�PID���ǲ�����
��������
���أ���
******************/
void PID_Test2()
{
  PID_Test_Time++;
  float time = 20;
  if(PID_Test_Time<time)
  {
    SpeedGoal = SpeedMin+PID_Test_Time*(SpeedMax-SpeedMin)/time;
  }
  else if(PID_Test_Time>=time && PID_Test_Time<time*2)
  {
    SpeedGoal = SpeedMax-(PID_Test_Time-time)*(SpeedMax-SpeedMin)/time;
  }
  else if(PID_Test_Time==time*2)
  {
    PID_Test_Time=0;
    SpeedGoal=SpeedMin;
  }
}
//void Brake()
//{
//  if(Speed)
//}