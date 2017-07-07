/***********************************************************
���к�֮                   �жϺ���
***********************************************************/
#include "common.h"
#include "include.h"
#include  "IRQ.h"

uint8 KeyTime=0;
uint8 SpeedTime=0;
uint8 VoltageTime=0;
uint8 Flag_Uart_Start=0;
uint8 Flag_Uart_Start2=0;
uint8 FlagStart = 0;

char uarttemp;

int Uart_Count = 0;
int Uart_Count2 = 0;
int VarSend[VarSendMax];
int VarReceive[VarReceiveMax];    //= {115,70,80,31,10,10,42,214,104,10,10,0,259,0,3500,2900,4150,75,100,22,8000,0,0,8000};
uint8 Uart_Buffer[(VarReceiveMax)*4+20];
uint8 Uart_Buffer2[VarExchangeMax+10];
extern reg_s ov7725_eagle_reg[];
extern uint8 new_tab;
extern ui_var_info_t  num_info[];
extern DMA_InitTypeDef DMA_InitStruct,DMA_InitStruct2;
uint8 GuaGua[7];
extern uint8 Sonic_AnotherState;
/*********************
���ã���ʱ���ж�
��������
���أ���
**********************/
void PIT1_IRQHandler()
{
  KeyTime++;
  SpeedTime++;
  VoltageTime++;
  if(KeyTime>=10)//8ms����һ��
  {
    KeyTime=0;
    key_IRQHandler();
  }

  if(SpeedTime>=5)
  {
    SpeedTime = 0;
    SpeedNow = -ftm_quad_get(FTM1);          //��ȡFTM �������� ��������(������ʾ������)
    ftm_quad_clean(FTM1);
    
    
//    Voltage = (adc_once(ADC0_SE18, ADC_16bit)*2.89*3300/((1<<16)-1));      //��ص�ѹ�ɼ�
//    Voltage_Alarm();                    //��ص�ѹ����

    if(CarState == CarStart)
    {
      DistanceRecord();
      SpeedAverage = (Distance*1000/(float)CarTime);
    //  PID_Test2();
     SpeedPID();
    //  BangBang();     
    if(SpeedFlag == Brake || SpeedFlag == Reversal_Slight){}
    else
      MotorOut(PWM_Motor);
     // SendWare();
    }
    if(CarState == CarStop)
      MotorOut(0); 
  }  
//  if(VoltageTime>500)
//  {
//    VoltageTime=0;
//    Voltage = (adc_once(ADC0_SE18, ADC_16bit)*2.89*3300/((1<<16)-1));      //��ص�ѹ�ɼ�
//    Voltage_Alarm();                    //��ص�ѹ����
//    
//  }
  if(CarState == CarStart)
  {
    CarTime++;
    //PWM_Motor=1500;
  }
  PIT_Flag_Clear(PIT1);
}
/*********************
���ã�����ͷ���ж�
��������
���أ���
**********************/
uint32 num;
void PORTC_IRQHandler()
{
  uint8  n;    //���ź�
  uint32 flag;
  
  //while(!PORTA_ISFR);
  flag = PORTC_ISFR;
  PORTC_ISFR  = ~0;                                   //���жϱ�־λ
  
  n = 15;                                             //���ж�
  if(flag & (1 << n))                                 //PTA29�����ж�
  {
    camera_vsync();
  }
#if ( CAMERA_USE_HREF == 1 )                            //ʹ�����ж�
  n = 28;
  if(flag & (1 << n))                                 //PTA28�����ж�
  {
    camera_href();
  }
#endif
  
}


void PORTD_IRQHandler()
{
    uint8  n = 8;    //���ź�
    if(PORTD_ISFR & (1 << n))        
    {
      PORTD_ISFR  = (1 << n);        //д1���жϱ�־λ
      if(CarType == Later)
      {
      Sonic_Calc();
      if(Sonic.state == 1)
      {
        Distance_TwoCar = (Sonic.Res-17);
        if(Distance_TwoCar<0)
          Distance_TwoCar = 0;
        if(Distance_TwoCar>255)
          Distance_TwoCar = 255;
      }
      else
        Distance_TwoCar = Distance_Far;
      }
    }
}
/*********************
���ã�����ͷDMA�ж�
��������
���أ���
**********************/
void DMA0_IRQHandler()
{
  camera_dma();
}
/*********************
���ã����ڷ���DMA�ж�
��������
���أ���
**********************/
void DMA1_IRQHandler()
{
  DMA_UART_Repeat(&DMA_InitStruct);
  DMA_IRQ_CLEAN(DMA_CH1);           //���ͨ�������жϱ�־λ
  
}
/*********************
���ã����ڷ���DMA�ж�
��������
���أ���
**********************/
void DMA2_IRQHandler()
{
  DMA_UART_Repeat(&DMA_InitStruct2);
  DMA_IRQ_CLEAN(DMA_CH2);           //���ͨ�������жϱ�־λ
}

/*********************
���ã����ڽ����ж�
��������
���أ���
**********************/
void uart0_handler(void)
{
    uint8 temp;
    if(uart_query    (UART0) == 1)   //�������ݼĴ�����
    {
        uart_getchar   (UART0, &temp);                    //���޵ȴ�����1���ֽ�
        if(Flag_Uart_Start2==1)
        { 
          Uart_Buffer2 [Uart_Count2++] = temp;
        }
        else
          Uart_Count2=0;
        if(temp == 0xff)                               //֡ͷ
          Flag_Uart_Start2 = 1; 
        if(Uart_Count2 == VarExchangeMax+1)
        {
          if(Uart_Buffer2[VarExchangeMax] == 254)       //֡β
          {
            if(Uart_Buffer2[0] == CarStart)
            {
              CarState = CarStart;
               StartSetZero();
            }
            if(Uart_Buffer2[0] == CarStop)
            {
              CarState = CarStop;
              ImgStopNum = ImgNum;
            }
            if(CarType == Front)
            {
              Distance_TwoCar = Uart_Buffer2[4];
              Sonic_AnotherState = Uart_Buffer2[5];
            }
            RoadType_AnotherCar = Uart_Buffer2[6];
          }
          Flag_Uart_Start2 = 0;
          Uart_Count2 = 0;
          memset(Uart_Buffer2,0,sizeof(Uart_Buffer2));
        }
    }
}
/*********************

���ã����ڽ����ж�
��������
���أ���
**********************/
void uart5_handler(void)
{
    
    if(uart_query    (UART5) == 1)   //�������ݼĴ�����
    {
        uart_getchar   (UART5, &uarttemp);                    //���޵ȴ�����1���ֽ�
        if(Flag_Uart_Start==1)
        { 
          Uart_Buffer [Uart_Count++] = uarttemp;
        }
        else
          Uart_Count=0;
        if(uarttemp == 'A')
          Flag_Uart_Start = 1; 
        if(uarttemp == 'B')
        {
          FlagStart = 1;
        }
        if(uarttemp == 'C')
        {
          FlagStart = 2;
        }
         if(uarttemp == 'D')
        {
          Flash_Write();
          ov7725_eagle_reg[45].val=VarReceive[17];
          camera_init(ImgAgo);
//          DELAY_MS(500);
//          Flash_Read();
//          Var_Init();        
        }
        if(Uart_Count == VarReceiveMax*4)
        {
          Flag_Uart_Start = 0;
          Uart_Count = 0;
          for(int i = 0; i<VarReceiveMax*4;i+=4)
          {
            VarReceive[i/4] = 1000*(Uart_Buffer[i]-0x30)+100*(Uart_Buffer[i+1]-0x30)+10*(Uart_Buffer[i+2]-0x30)+Uart_Buffer[i+3]-0x30;
          }
          memset(Uart_Buffer,0,sizeof(Uart_Buffer));
          VarConfirm_Uart();
        }
    }
}


