#ifndef _OLDBOY_IMG_DEAL_H_
#define _OLDBOY_IMG_DEAL_H_
void BottomFind();                        //ͼ��ײ�Ѱ�ң�ȷ����׼ֵ
void GenZong();                          //�ҵ��߽���
void jisuan_Middle();                    //��������
void ForeSight();                        //ȷ��ǰհ
uint8 Refind_L(uint8 y);                 //����һ����߽�
uint8 Refind_R(uint8 y);                 //����һ���ұ߽�
int  judge_crossxieR(uint8 judge);      //��б��ʮ���жϼ�����
int  judge_crossxieL(uint8 judge);      //��б��ʮ���жϼ�����
void judge_road();                      //�ж���������
float jisuan_piancha();                 //����ͼ��ƫ��
void StartLineCheck();                  //�����߼��
void StraightCheck();                   //ֱ���ж�
void GoInBendCheck();                   //�����ж�
void CrossCheck();                      //ʮ������
uint8 OvertakeAreaFind(uint8 judge);   //�ӿ��������
uint8 OvertakeArea_ing(uint8 judge);        //���볬����
uint8 OvertakeArea_Return(uint8 judge);//�ӳ���������
void Overtake_LaterToFront();//�������󳵱�ǰ��
void Overtake_LaterReturn();//�󳵴ӳ���������
void OvertakeArea_Over();   //��������
void Overtake_FrontReturn();//ǰ��������������
void Overtake_Middle();
void CurveType_Roll();                   //�������͹�������
void GenZong_Again_Right();             //ǰ���ͱ߽����һ���ٴ�Ѱ��
void GenZong_Again_Left();
void Left_Correct(uint8 y);
void Right_Correct(uint8 y);
void Middle_Triangle();                 //���ǳ��������߼���
void OutsideCheck();
void LCD_Find();
#endif 