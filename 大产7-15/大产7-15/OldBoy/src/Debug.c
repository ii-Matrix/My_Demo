/***********************************************************
老男孩之                    调试
***********************************************************/
#include "common.h"
#include "include.h"
#include  "Debug.h"
extern DMA_InitTypeDef DMA_InitStruct,DMA_InitStruct2;
extern reg_s ov7725_eagle_reg[];
uint32  CarState = CAR_NULL;                            //车的状态
uint32 Time_Duoji,Time_SD,Time_Camera,Time_Suanfa,Time_1,Time_2,Time_3,Time_4,Time_5;      //相应函数执行时间
float Voltage = 8000.0;                                         //电池电压
uint8 CarFlag;                                         //车子运行状态标志位
uint8 Uart_Send_Buffer[VarSendMax*2+2];
uint8 Nrf_Tx[14];                                       //Nrf发送缓冲区
uint8 Nrf_Rx[14];                                       //Nrf接收缓冲区
Car_Type CarType;
IMG_TX_SELECT IMG_TX;
nrf_result_e nrf_result;
extern ui_var_info_t  num_info[];
uint8 VarExchange[VarExchangeMax+2];
uint8 Sonic_AnotherState;
//int16 Uart_Send_Buffer[VarSendMax+2];
/******************
作用：拨码开关初始化
参数：无
返回：无
******************/

void Boma_init()
{
  gpio_init (Boma_1, GPI,0);
  port_init_NoALT(Boma_1, PULLUP);         //配置为上拉
  gpio_init (Boma_2, GPI,0);
  port_init_NoALT(Boma_2, PULLUP);         //配置为上拉
  gpio_init (Boma_3, GPI,0);
  port_init_NoALT(Boma_3, PULLUP);         //配置为上拉
  gpio_init (Boma_4, GPI,0);
  port_init_NoALT(Boma_4, PULLUP);         //配置为上拉
  gpio_init(Boma_5, GPI, 0);
  port_init_NoALT(Boma_5, PULLUP);         //保持复用不变，仅仅改变配置选项
  
  gpio_init (Boma_6, GPI,0);
  port_init_NoALT(Boma_6, PULLUP);         //配置为上拉
}

void Beep_Welcome()
{
#define Beep_Welcom_Time 50
  for(int i= 0;i<=10;i++)
  {
    ftm_pwm_init(FTM3, FTM_CH1,i*1000,5000);
    DELAY_MS(Beep_Welcom_Time);
  }
  for(int i= 10;i>=0;i--)
  {
    ftm_pwm_init(FTM3, FTM_CH1,i*1000,5000);
    DELAY_MS(Beep_Welcom_Time);
  }  
}
/******************
作用：读取flash值，为参数赋值
参数：无
返回：无
******************/
void Flash_Read()
{
  if(gpio_get (Boma_1) == 0)
  {
    for(uint8 i=0;i<VarReceiveMax;i++)
    {
      VarReceive[i] = flash_read(SECTOR_NUM-1,i*8, uint32);  
    }   
  }
  else if(gpio_get (Boma_2) == 0)
  {
    for(uint8 i=0;i<VarReceiveMax;i++)
    {
      VarReceive[i] = flash_read(SECTOR_NUM-2,i*8, uint32);  
    }     
  }
  else if(gpio_get (Boma_3) == 0)
  {
    for(uint8 i=0;i<VarReceiveMax;i++)
    {
      VarReceive[i] = flash_read(SECTOR_NUM-3,i*8, uint32);  
    }     
  }       
}
/******************
作用：存储变量到Flash
参数：无
返回：无
******************/
void Flash_Write()
{
  if(gpio_get (Boma_1) == 0)
  {
    flash_erase_sector(SECTOR_NUM-1);                     //擦除扇区
    for(uint8 i = 0;i<VarReceiveMax;i++)
    {
      flash_write(SECTOR_NUM-1,i*8,num_info[i].val);
    } 
  }
  else if(gpio_get (Boma_2) == 0)
  {
    flash_erase_sector(SECTOR_NUM-2);                     //擦除扇区
    for(uint8 i = 0;i<VarReceiveMax;i++)
    {
      flash_write(SECTOR_NUM-2,i*8,num_info[i].val);
    }    
  }
  else if(gpio_get (Boma_3) == 0)
  {
    flash_erase_sector(SECTOR_NUM-3);                     //擦除扇区
    for(uint8 i = 0;i<VarReceiveMax;i++)
    {
      flash_write(SECTOR_NUM-3,i*8,num_info[i].val);
    }    
  }    
}
void Var_Init()
{  
  SpeedMax                 = VarReceive[0];
  SpeedMid                 = VarReceive[1];
  SpeedMin                 = VarReceive[2];
  SpeedChase_Range         = VarReceive[3];
  SpeedChase_KP            = VarReceive[4];
  SpeedTriangle            = VarReceive[5];
  DP_A                     = VarReceive[6];         
  DP_C                     = VarReceive[7];
  Duoji_D                  = VarReceive[8];
  Duoji_D_A                = VarReceive[9];
  Duoji_DividingLine       = VarReceive[10];
  Duoji_P_Cross            = VarReceive[11];
  Duoji_D_Cross            = VarReceive[12];         
  ImgCurveDegree           = VarReceive[13]/10000.0;
  Dj_Mid                   = VarReceive[14];
  Dj_Left                  = VarReceive[15];
  Dj_Right                 = VarReceive[16];
  ov7725_eagle_reg[45].val = VarReceive[17];
  Motor_P                  = VarReceive[18];
  Motor_I                  = VarReceive[19];
  Kp_DuojiCorrect          = VarReceive[20];
  PWM_Overtake             = VarReceive[21];
  Kv                       = VarReceive[22];
  Img_P[0]=1;
  Img_P[1]=1;
  Img_P[2]=1;
  Img_P[3]=1;
  Img_P[4]=1;
  Img_P[5]=1;
  Img_P[6]=1;
  Img_P[7]=1;
  Img_P[8]=1;
  Img_P[9]=1;
  Img_P[10]=1;
  Img_P[11]=1;
  
  
  //  SelectRowFlag[119] = 1;
  //  SelectRowFlag[113] = 1;
  //  SelectRowFlag[110] = 1;
  //  SelectRowFlag[107] = 1;
  //  SelectRowFlag[104] = 1;
  //  SelectRowFlag[102] = 1;
  //  SelectRowFlag[100] = 1;
  //  SelectRowFlag[97] = 1;
  //  SelectRowFlag[95] = 1;
  //  SelectRowFlag[93] = 1;
  //  SelectRowFlag[91] = 1;
  //  SelectRowFlag[89] = 1;
  //  SelectRowFlag[88] = 1;
  //  SelectRowFlag[86] = 1;
  //  SelectRowFlag[84] = 1;
  //  SelectRowFlag[83] = 1;
  //  SelectRowFlag[81] = 1;
  //  SelectRowFlag[80] = 1;
  //  SelectRowFlag[79] = 1;
  //  SelectRowFlag[78] = 1;
  //  SelectRowFlag[77] = 1;
  //  SelectRowFlag[76] = 1;
  //  SelectRowFlag[75] = 1;
  //  SelectRowFlag[74] = 1;    
  //  SelectRowFlag[73] = 1;
  //  SelectRowFlag[72] = 1;
  //  SelectRowFlag[71] = 1;
  //  SelectRowFlag[70] = 1;
  //  SelectRowFlag[69] = 1;
  //  SelectRowFlag[68] = 1;
  //  SelectRowFlag[67] = 1;
  //  SelectRowFlag[66] = 1;
  //  SelectRowFlag[65] = 1;
  //  SelectRowFlag[64] = 1;
  //  SelectRowFlag[63] = 1;
  //  SelectRowFlag[62] = 1;
  //  SelectRowFlag[61] = 1;
  //  SelectRowFlag[60] = 1;
  //  SelectRowFlag[59] = 1;
  //  SelectRowFlag[58] = 1;
  //  SelectRowFlag[57] = 1;
  //  SelectRowFlag[56] = 1;
  //  SelectRowFlag[55] = 1;
  //  SelectRowFlag[54] = 1;
  //  SelectRowFlag[53] = 1;
  //  SelectRowFlag[52] = 1;
  //  SelectRowFlag[51] = 1;
  //  SelectRowFlag[50] = 1;
  //  SelectRowFlag[49] = 1;
  //  SelectRowFlag[48] = 1;
  //  SelectRowFlag[47] = 1;
  //  SelectRowFlag[46] = 1;
  //  SelectRowFlag[45] = 1;
  //  SelectRowFlag[44] = 1;
  //  SelectRowFlag[43] = 1;
  //  SelectRowFlag[42] = 1;
  //  SelectRowFlag[41] = 1;
  //  SelectRowFlag[40] = 1;
  //  SelectRowFlag[39] = 1;
  //  SelectRowFlag[38] = 1;
  //  SelectRowFlag[37] = 1;
  //  SelectRowFlag[36] = 1;
  //  SelectRowFlag[35] = 1;
  //  SelectRowFlag[34] = 1;
  //  SelectRowFlag[33] = 1;
  //  SelectRowFlag[32] = 1;
  //  SelectRowFlag[31] = 1;
  //  SelectRowFlag[30] = 1;
  //  SelectRowFlag[29] = 1;
  //  SelectRowFlag[28] = 1;
  //  SelectRowFlag[27] = 1;
  //  SelectRowFlag[26] = 1;
  //  SelectRowFlag[25] = 1;
  //  
  SpeedFlag = Speed_Null;
  
  
}
/******************
作用：电池低压报警
参数：无
返回：无
******************/
void Voltage_Alarm()
{
  if(CarState == CarStop)
  {
    if(Voltage<6900)
    {
      ftm_pwm_init(FTM3, FTM_CH1,4*1000,5000);
    }
  }
}
/******************
作用：发送速度曲线
参数：无
返回：无
******************/
void SendWare()
{
  if(CarState==CarStart)
  {
#define CMD_WARE     3
    uint16 speed[3];
    speed[0]=(uint16)SpeedGoal;
    speed[1]=(uint16)SpeedNow;
    speed[2]=(uint16)PWM_Motor;
    uint8 cmdf[2] = {CMD_WARE, ~CMD_WARE};    //yy_摄像头串口调试 使用的命令
    uint8 cmdr[2] = {~CMD_WARE, CMD_WARE};    //yy_摄像头串口调试 使用的命令
    
    uart_putbuff(UART5, cmdf, sizeof(cmdf));    //先发送命令
    
    uart_putbuff(UART5, (uint8 *)speed, sizeof(speed)); //再发送图像
    
    uart_putbuff(UART5, cmdr, sizeof(cmdr));    //先发送命令
    
    // UART_SendWithDMA(&DMA_InitStruct, ( uint8*)speed, sizeof(speed));  
    //   uart_putbuff(UART5, (uint8 *)speed, sizeof(speed));
  }
}
/******************
作用：交换参数
******************/
void Exchange_Var()
{

  VarExchange[0] = 255;                    //帧头
  VarExchange[1] = 8;
  VarExchange[2] = SpeedGoal;
  VarExchange[3] = SpeedNow;
  VarExchange[4] = SpeedFlag;
  VarExchange[5] = Distance_TwoCar;
  VarExchange[6] = Sonic.state;
  VarExchange[7] = RoadType;
  VarExchange[VarExchangeMax+1] = 254;    //帧尾
   for(uint8 i = 1;i<VarExchangeMax+1;i++)  //限幅
  {
    if(VarExchange[i]>253)
      VarExchange[i] = 253;
  }  
  if(FlagStart == 1)
  {
    FlagStart = 0;
    VarExchange[1] = CarStart;
    UART_SendWithDMA(&DMA_InitStruct2, ( uint8*)VarExchange, VarExchangeMax+2); 
    StartSetZero();
    CarState = CarStart;
  }
  else
  {
    if(FlagStart == 2)
    {
      FlagStart = 0;
      CarState = CarStop;
      ImgStopNum = ImgNum;
      VarExchange[1] = CarStop;
    }

    UART_SendWithDMA(&DMA_InitStruct2, ( uint8*)VarExchange, VarExchangeMax+2); 
  }
}

/******************
作用：发送参数
参数：无
返回：无
******************/
void Send_Var()
{
  VarSend[0] = MiddleError*10;
  VarSend[1] = PWM_Duoji-Dj_Mid;
  VarSend[2] = Sight;
  VarSend[3] = Sight_L;
  VarSend[4] = Sight_R;
  VarSend[5] = Duoji_P*10;
  VarSend[6] = SpeedGoal;
  VarSend[7] = SpeedNow;
  VarSend[8] = PWM_Motor;
  VarSend[9] = Distance;
  VarSend[10] = Time_Camera;
  VarSend[11] = Time_Suanfa;
  VarSend[12] = Distance_TwoCar;
  VarSend[13] = RoadType;
  VarSend[14] = RoadType_AnotherCar;
  VarSend[15] = SpeedFlag;
  VarSend[16] = CarType; 
  VarSend[17] = (int)UP;
  VarSend[18] = (int)UD;
  VarSend[19] = RoadScan.Res.state;
  VarSend[20] = Light_X;
  VarSend[21] = Light_Y;
  VarSend[22] = Speed_P;
  VarSend[23] = Speed_I;
  VarSend[24] = Voltage;
  Uart_Send_Buffer[0] = 'A';
  for(int i = 0;i<VarSendMax;i++)
  {
    Uart_Send_Buffer[i*2+1] = (uint8)((VarSend[i]&0x0000ffff)>>8);  //整形的数组截取了后两个字节，高8位
    Uart_Send_Buffer[i*2+2] = (uint8)((VarSend[i]&0x0000ffff));  //低8位   
    if(VarSend[i]<0)
      Uart_Send_Buffer[i*2+1] |= 0x80;
    else
      Uart_Send_Buffer[i*2+1] &= 0x7f;
  }
  
  
  Uart_Send_Buffer[VarSendMax*2+1] = 'B';        //帧尾
  int YuZhi = 150;
  //   if((CarState==CarStop && ImgStopNum == 0) ||(CarState == CarStop && ImgNum-ImgStopNum>YuZhi) || ((CarState == CarStart) && (gpio_get (Boma_6)==1)))
  //if((CarState==CarStop && ImgStopNum == 0) ||(CarState == CarStop && ImgNum-ImgStopNum>=YuZhi))
  
  {
    //    uart_putbuff (UART5,( uint8*)Uart_Send_Buffer, sizeof(Uart_Send_Buffer));
  }
  //    if(nrf_result == NRF_RESULT_RX_NO)
        UART_SendWithDMA(&DMA_InitStruct, ( uint8*)Uart_Send_Buffer, sizeof(Uart_Send_Buffer));  
}

/******************
作用：启动时参数清零
参数：无
返回：无
******************/
void StartSetZero()
{
  Distance=0;
  Distance_StartLine=0;
  Distance_Cross = 0;
  Distance_GoOutBend = 0;
  CarTime=0;
  SpeedAverage=0;
  SpeedFlag = Speed_Null;
  Count_Total=0;
  ImgStopNum = 0;
  CrossNum = 0;
  StraightNum = 0;
  CurveType = Curve_Null;
  RoadType = Normal;
  Flag_LCD_White=1;
  LCD_clear(WHITE); 
}
void NRF_TX()
{
  if(IMG_TX == OFF)
  {
    Nrf_Tx[2] = CarState;
    Nrf_Tx[3] = RoadType;
    nrf_msg_tx(COM_CarVar,Nrf_Tx);
    while(nrf_tx_state() == NRF_TXING);
  }
  
}
void NRF_TX_IMG()
{
  
}
void NRF_RX()
{
  if(IMG_TX == OFF)
  {
    com_e     com;
    
    do
    {
      nrf_result = nrf_msg_rx(&com, Nrf_Rx);
      if(nrf_result == NRF_RESULT_RX_VALID)
      {
        led_turn(LED0);
        switch(com)
        {
        case COM_CarVar:
          //            if(CarType == Front)
          //            {
          //               //   if(Nrf_Rx[2] == CarStart)
          //                  {
          //                    CarState = CarStart;
          //                //    StartSetZero();
          //                  }              
          //            }
          //   if(Nrf_Rx[2] != CarState)
          {
            if(Nrf_Rx[2] == CarStart)
            {
              CarState = CarStart;
              StartSetZero();
            }
            if(Nrf_Rx[2] == CarStop)
            {
              CarState = CarStop;
              ImgStopNum = ImgNum;
            }                   
          }
          RoadType_AnotherCar = Nrf_Rx[3];
          //              if(RoadType_AnotherCar == Overtake_Right_ing)
          //                CarType = Front;
          break;
        }
      }
    }while(nrf_result != NRF_RESULT_RX_NO);
  }
}
void Beep_ON()
{
  if(CarState == CarStart)
  {
    ftm_pwm_duty(FTM3, FTM_CH1,6000);
    
  }
  else
    ftm_pwm_duty(FTM3, FTM_CH1,0);
}
void Beep_OFF()
{
  ftm_pwm_duty(FTM3, FTM_CH1,0);
}
void VarConfirm_Uart()
{
  num_info[0].val  = SpeedMax                 = VarReceive[0];
  num_info[1].val  = SpeedMid                 = VarReceive[1];
  num_info[2].val  = SpeedMin                 = VarReceive[2];
  num_info[3].val  = SpeedChase_Range         = VarReceive[3];
  num_info[4].val  = SpeedChase_KP            = VarReceive[4];
  num_info[5].val  = SpeedTriangle            = VarReceive[5];
  num_info[6].val  = DP_A                     = VarReceive[6];         
  num_info[7].val  = DP_C                     = VarReceive[7];
  num_info[8].val  = Duoji_D                  = VarReceive[8];
  num_info[9].val  = Duoji_D_A                = VarReceive[9];
  num_info[10].val = Duoji_DividingLine       = VarReceive[10];
  num_info[11].val = Duoji_P_Cross            = VarReceive[11];
  num_info[12].val = Duoji_D_Cross            = VarReceive[12];                                                   
  num_info[13].val = ImgCurveDegree           = VarReceive[13];
  num_info[14].val = Dj_Mid                   = VarReceive[14];
  num_info[15].val = Dj_Left                  = VarReceive[15];
  num_info[16].val = Dj_Right                 = VarReceive[16];
  num_info[17].val = ov7725_eagle_reg[45].val = VarReceive[17];
  num_info[18].val = Motor_P                  = VarReceive[18];
  num_info[19].val = Motor_I                  = VarReceive[19];
  num_info[20].val = Kp_DuojiCorrect          = VarReceive[20];
  num_info[21].val = PWM_Overtake             = VarReceive[21];
  num_info[22].val = Kv                       = VarReceive[22];
  var_syn(VAR_MAX);
}
void VarConfirm_Key()
{
  var_syn(VAR_MAX);
  SpeedMax                 = VarReceive[0];
  SpeedMid                 = VarReceive[1];
  SpeedMin                 = VarReceive[2];
  SpeedChase_Range         = VarReceive[3];
  SpeedChase_KP            = VarReceive[4];
  SpeedTriangle            = VarReceive[5];
  DP_A                     = VarReceive[6];         
  DP_C                     = VarReceive[7];
  Duoji_D                  = VarReceive[8];
  Duoji_D_A                = VarReceive[9];
  Duoji_DividingLine       = VarReceive[10];
  Duoji_P_Cross            = VarReceive[11];
  Duoji_D_Cross            = VarReceive[12];                                                   
  ImgCurveDegree           = VarReceive[13];
  Dj_Mid                   = VarReceive[14];
  Dj_Left                  = VarReceive[15];
  Dj_Right                 = VarReceive[16];
  ov7725_eagle_reg[45].val = VarReceive[17];
  Motor_P                  = VarReceive[18];
  Motor_I                  = VarReceive[19]; 
  Kp_DuojiCorrect          = VarReceive[20];
  PWM_Overtake             = VarReceive[21];
  Kv                       = VarReceive[22];
}
/****************************
作用：参数清零
****************************/
void SetZero()
{
  memset(Nrf_Tx,0,sizeof(Nrf_Tx));
  memset(Nrf_Rx,0,sizeof(Nrf_Rx));
  memset(Left,Left_Init,120);
  memset(Middle,80,120);
  memset(Right,Right_Init,120);
  memset(Lost_L,0,120);
  memset(Lost_R,0,120); 
  Lost_L_Num=Lost_R_Num=0;
  Sight_Last = Sight;
  Sight=Sight_Init;
  Sight_L=Sight_L_Init;
  Sight_R=Sight_R_Init;
  Last_L=Left_Init;
  Last_R=Right_Init;
  
  ValidLine=0;
  ErrorType = NoError;
  
  if(CarState == CarStop)
  {
    Distance_StartLine = 0;
  }
  //if(RoadType == GoOutBend || RoadType == Cross)
  //  if(RoadType == GoInBend || RoadType == Cross)
  //  if(RoadType == Overtake_Left || RoadType == Overtake_Left_ing || RoadType == Overtake_Left_Return || RoadType == Overtake_Right || RoadType == Overtake_Right_ing || RoadType == Overtake_Right_Return)
  if(CarType == Front)
  {
    if(RoadType == Overtake_Right)
    {
    // if(RoadType == GoOutBend || RoadType == Cross)
    //  if(CurveType == Straight)
    //    if(SpeedFlag == Speed_Up)
    //        Beep_ON();
     led(LED1,LED_ON);
    }
    else
    {
    //        Beep_OFF();
     led(LED1,LED_OFF);
    }
  }
  CurveType_Roll();//弯曲程度滚动更新 
  CurveType = Curve_Null;
  RoadType_Last = RoadType;
  if(RoadType == Cross){}
  else if(RoadType == Overtake_Left){}
  else if(RoadType == Overtake_Right){}
  else if(RoadType == Overtake_Left_ing){}
  else if(RoadType == Overtake_Left_Return){}
  else if(RoadType == Overtake_Right){}
  else if(RoadType == Overtake_Right_ing){}
  else if(RoadType == Overtake_Right_Return){}
  else if(RoadType == Overtake_Right_GoStraight){}
  else if(RoadType == Overtake_Right_Rerturn_Prepare){}
  else if(RoadType == Overtake_Right_Rerturn_Execute){}
  else 
    RoadType=Normal;
  if(RoadType_AnotherCar == Cross){}
  else if(RoadType_AnotherCar == Overtake_Left){}
  else if(RoadType_AnotherCar == Overtake_Right){}
  else if(RoadType_AnotherCar == Overtake_Left_ing){}
  else if(RoadType_AnotherCar == Overtake_Left_Return){}
  else if(RoadType_AnotherCar == Overtake_Right){}
  else if(RoadType_AnotherCar == Overtake_Right_ing){}
  else if(RoadType_AnotherCar == Overtake_Right_Return){}
  else if(RoadType_AnotherCar == Overtake_Right_GoStraight){}
  else if(RoadType_AnotherCar == Overtake_Right_Rerturn_Prepare){}
  else if(RoadType_AnotherCar == Overtake_Right_Rerturn_Execute){}
  else 
    RoadType_AnotherCar=Normal;
  if(CarType == Later)
  {
//    if(RoadType_AnotherCar == Overtake_Left)
//      led(LED0,LED_ON);
//    else
//      led(LED0,LED_OFF);
    if(Distance_TwoCar<=Distance_Near)
      Beep_ON();
    else
      Beep_OFF();
  }
  
}