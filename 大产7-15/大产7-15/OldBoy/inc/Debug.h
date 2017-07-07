#ifndef _DEBUG_H_
#define _DEBUG_H_

#define Boma_1 PTC9
#define Boma_2 PTC8
#define Boma_3 PTC7
#define Boma_4 PTC6
#define Boma_5 PTC5
#define Boma_6 PTC4
void Boma_init();       //拨码开关初始化
void Flash_Read();      //flash读取，为参数赋值
void Flash_Write();     //存储变量到flash
void Beep_Welcome();      //开机声音
void Var_Init();        //变量初始化
void Voltage_Alarm();   //电池低压报警
void SendWare();        //发送速度曲线
void Send_Var();        //发送变量
void uart5_handler(void);   //串口中断处理
void StartSetZero();    //启动时的参数清零
void NRF_TX();          //NRF发送
void NRF_RX();          //NRF接收
void Beep_ON();         //蜂鸣器响
void Beep_OFF();        //蜂鸣器不响
void VarConfirm_Uart();      //变量更新
void VarConfirm_Key();
void Exchange_Var();          //交换参数
void SetZero();                         //参数清零

#endif 