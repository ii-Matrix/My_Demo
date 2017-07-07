#ifndef _DEBUG_H_
#define _DEBUG_H_

#define Boma_1 PTC9
#define Boma_2 PTC8
#define Boma_3 PTC7
#define Boma_4 PTC6
#define Boma_5 PTC5
#define Boma_6 PTC4
void Boma_init();       //���뿪�س�ʼ��
void Flash_Read();      //flash��ȡ��Ϊ������ֵ
void Flash_Write();     //�洢������flash
void Beep_Welcome();      //��������
void Var_Init();        //������ʼ��
void Voltage_Alarm();   //��ص�ѹ����
void SendWare();        //�����ٶ�����
void Send_Var();        //���ͱ���
void uart5_handler(void);   //�����жϴ���
void StartSetZero();    //����ʱ�Ĳ�������
void NRF_TX();          //NRF����
void NRF_RX();          //NRF����
void Beep_ON();         //��������
void Beep_OFF();        //����������
void VarConfirm_Uart();      //��������
void VarConfirm_Key();
void Exchange_Var();          //��������
void SetZero();                         //��������

#endif 