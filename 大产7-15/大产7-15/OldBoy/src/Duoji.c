/***********************************************************
���к�֮             �������
***********************************************************/
#include "common.h"
#include "include.h"
#include  "Duoji.h"
//#if (CarName == 1)
//int Dj_Mid   = 4775;
//int Dj_Left  = 3825;
//int Dj_Right = 5725;
//#endif
//#if (CarName == 2)
//int Dj_Mid   = 4350;
//int Dj_Left  = 3400;
//int Dj_Right = 5300;
//#endif

int Dj_Mid   = 3550;
int Dj_Left  = 2800;
int Dj_Right = 4350;

float Duoji_P,Duoji_P_Cross,Duoji_D,Duoji_D_Cross,Duoji_E,UP,UP_Max,UD,UD_Now,UD_Last,UD_Old,Hk1,Hk;  //���P,D
float DP_A,DP_B,DP_C;   //�����̬P�Ĳ���
float Duoji_D_A;
float Derivative_Now,Derivative_Last;//΢����
int PWM_Duoji;
uint8 Duoji_DividingLine;    //�������ߺͶ�ֵP�ķֽ���
float Kp_DuojiCorrect;
void Duoji()
{
//  if(RoadType == Cross)
//  {
//    UP=Duoji_P_Cross/10.0*MiddleError; 
//    UD=Duoji_D_Cross*(MiddleError-MiddleError_Last);
//  }
//  else
  {
    if(AbsMiddleError<=Duoji_DividingLine)
    {
      Duoji_P=DP_C/10.0;
    }
    else
    {
      Duoji_P=(AbsMiddleError*AbsMiddleError)*(DP_A/10000.0)+DP_C/10.0;
    }
      if(Duoji_P>50)
        Duoji_P=50;
    
      UP=Duoji_P*MiddleError;
      
    Derivative_Now  = MiddleError - MiddleError_Last;
    Derivative_Last = MiddleError_Last - MiddleError_Previous;
    
    UD=Duoji_D*Derivative_Now*Duoji_D_A/10.0+Duoji_D*Derivative_Last*(1.0-Duoji_D_A/10.0);
   
    MiddleError_Previous = MiddleError_Last;
    MiddleError_Last = MiddleError; 
    //P���޷���Ҫʵ����ǰ������Ҫ����P��Ȼ�������P�ڳ���ʱ��ת��ͷ��
    //��ΪP��ʵ�ڹ��󣬵���΢�ֶԶ�������Ĺ��ײ������ã���������Ǵ���״̬��Ȼ���ת��ͷ�ˡ�
    if(UP>Dj_Right-Dj_Mid)
      UP = Dj_Right-Dj_Mid;
    if(UP<(Dj_Left-Dj_Mid))
      UP = Dj_Left-Dj_Mid;
    if(UD>700)
       UD = 700;
    if(UD<-700)
      UD = -700;
  }
//  if(RoadType == Overtake_Right_Rerturn_Prepare){}
//  else
  {
    if(MiddleError<0)  //��ת
      PWM_Duoji=(int)(Dj_Mid+UP+UD);
    else
      // PWM_Duoji=(int)(Dj_Mid+UP+UD);
      PWM_Duoji=(int)(Dj_Mid+Kp_DuojiCorrect/100.0*(UP+UD));
  }
  if(PWM_Duoji<Dj_Left)
    PWM_Duoji=Dj_Left;
  if(PWM_Duoji>Dj_Right)
    PWM_Duoji=Dj_Right;
  int YuZhi = 150;
  if((CarState==CarStart) || (CarState == CarStop && ImgStopNum !=0 && ImgNum-ImgStopNum<YuZhi)) 
    ftm_pwm_duty(FTM2, FTM_CH0,PWM_Duoji);
  if((CarState==CarStop && ImgStopNum == 0) ||(CarState == CarStop && ImgNum-ImgStopNum>=YuZhi)) 
    ftm_pwm_duty(FTM2, FTM_CH0,Dj_Mid);
//  if(CarState==CarStop)
////    Beep_OFF();
}


