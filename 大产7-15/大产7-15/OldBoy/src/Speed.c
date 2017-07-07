/***********************************************************
老男孩之               速度控制
***********************************************************/
#include "common.h"
#include "include.h"
#include  "Speed.h"
#include "Var.h"
int SpeedMax;           //最大速度
int SpeedMid;           //中间速度
float SpeedChase_Range;   //追逐速度变化范围
int SpeedChase_KP;      //追逐速度变化系数
int SpeedTriangle;      //三角超车区后车速度
int SpeedMin;                   //最小速度
int SpeedGoal;                  //目标速度
int SpeedGoal_AnotherCar;       //另一辆车速度
int Speed_P;                    //速度P项
int Speed_I;                    //速度I项
int Motor_P;                    //电机P参数
int Motor_I;                    //电机I参数
int MotorOutMax;                //电机最大输出PWM
int MotorOutMin=0;              //电机最小输出PWM
int16 SpeedNow;                 //当前速度
int PWM_BangBangMax;            //bangbang输出上限
int PWM_BangBangMin;            //bangbang输出下限
int BangBang_Yuzhi_Up;             //bangbang阈值
int BangBang_YuZhi_Down;
int Flag_BangBang;              //bangbang标志位
int MotorError[3];              //电机偏差
int PWM_Motor;                  //电机PWM
int CarTime;                    //车子运行一圈的时间
int SpeedAverage;               //一圈结束后平均速度
int PID_Test_Time;              //速度PID矩形波测试时间
float Distance,Distance_Last;                 //路程，单位：厘米
float Distance_StartLine;       //起跑线路程记录
float Distance_Overtake;
float Distance_GoOutBend;       //出弯路程记录
float Distance_GoInBend;
float Distance_Cross,Distance_Cross_GoOut;
float Distance_Overtake_Right_GoStraight;
float Distance_Overtake_Left_GoStraight;
extern uint8 Sonic_AnotherState;
long Count_Total,Count_Total_Last;               //脉冲累计值
int Distance_TwoCar;          //双车距离
uint8 Flag_Brake;               //刹车标志位
Speed_Flag SpeedFlag,SpeedFlag_Last;           //速度控制标志位
int ReversalPower_Slight = -700;              //反转力度
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
未验证
作用：速度控制
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
//      else                //反接制动
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
作用：电机输出PWM
参数：计算好的PWM
****************************/
void MotorOut(int PWMOut)
{
  if(CarState == CarStart && Distance<200 && SpeedNow<SpeedGoal*0.96)//最初的一米，最高限幅4000，为了更快的启动。
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
    if(PWMOut > MotorOutMax)             //限幅      
      PWMOut = MotorOutMax ;
    else if(PWMOut < ReversalPower_Slight)
      PWMOut = ReversalPower_Slight ;    
  }
  else if(SpeedFlag == Reversal_Urgency)
  {
    if(PWMOut > MotorOutMax)             //限幅      
      PWMOut = MotorOutMax ;
    else if(PWMOut < ReversalPower_Urgency)
      PWMOut = ReversalPower_Urgency ;    
  }
  else
  {
    if(PWMOut > MotorOutMax)             //限幅      
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
作用：增量式PID
****************************/
void SpeedPID()    
{
  MotorError[2]=MotorError[1];
  MotorError[1]=MotorError[0];
  MotorError[0]=SpeedGoal-SpeedNow;
  Speed_P = Motor_P*(MotorError[0]-MotorError[1]);
  Speed_I = Motor_I*(MotorError[0]);
//  if(Speed_I>200)               //积分限幅
//  {
//    Speed_I = 200;
//  }
//  else if (Speed_I <-200)
//  {
//    Speed_I = -200;
//  }
  if(abs(MotorError[0])>1)             //减弱抖动
    PWM_Motor += (Speed_P + Speed_I);
 // PWM_Motor = PWM_Motor*8000.0/Voltage;//电压校正

//  if(PWM_Motor>MotorOutMax*8000.0/Voltage)      
//    PWM_Motor=MotorOutMax*8000.0/Voltage;
    if(PWM_Motor>MotorOutMax)      
    PWM_Motor=MotorOutMax;
  if(PWM_Motor<MotorOutMin)
    PWM_Motor=MotorOutMin;
}
/****************************
作用：路程记录
****************************/
void DistanceRecord()
{
  Count_Total+=SpeedNow;
  Distance = (float)Count_Total/67.0;
}
/****************************
作用：bangbang控制
****************************/
void BangBang()
{
  int Error = SpeedGoal - SpeedNow; 
  BangBang_Yuzhi_Up = 10; 
  BangBang_YuZhi_Down = -7;
  if(Error>0 && Sight - Sight_Init<7)            // 急加速
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
//  else if(Error<0 && abs(Error)>BangBang_Yuzhi)//  急减速
//  {
//    PWM_Motor = MotorOutMax;
//  }
}
/****************************
作用：电机开环运行
****************************/
void KaiHuan()
{
  if(RoadType == Straight)     
  {
    PWM_Motor = SpeedMax;
  }
  else if(RoadType == Bend)
  {
    PWM_Motor =  SpeedMin+BendNum*2;//弯道逐渐加速
  }
  else  //速度二次曲线，速度设定方案的核心，然而我并不会。。。
        //大概看了看计数报告，有两种方案，一种是和最远前瞻有关，一种是图像偏差有关，可能要用MATLAB或EXCEL拟合二次曲线
        //然后，根据不同的赛道，动态改变下列公式中的速度最大值和最小值
  {
  //  PWM_Motor = SpeedMax - (SpeedMax-SpeedMin)
  }
}
/******************
作用：速度PID矩形波测试
参数：无
返回：无
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
作用：速度PID三角波测试
参数：无
返回：无
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