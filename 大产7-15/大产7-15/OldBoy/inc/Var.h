#ifndef _OLDBOY_VAR_H_
#define _OLDBOY_VAR_H_
//˵��������һЩ���Ʋ������Ǵ�flash��ȡ�ģ�������Щ��������ʱ��û�и���ֵ�����ǰΪ�˱���������Ǹ�����ֵ�ɣ���һ��Ƭflash�������Ĵ���̫�࣬������ô�졣

/******************����ͷ��ͼ��*****************/
typedef enum           //��������
{
  Normal,                 //����                    0
  GoOutBend,             //����                     1
  Cross,                 //����ʮ��                 2
  CrossL,                //б��ʮ�֣�����ת         3
  CrossR,                //б��ʮ�֣�����ת         4
  StartLine,             //������                  5
  Obstacle_L,            //���ϰ�                  6
  Obstacle_R,             //���ϰ�                 7
  UpHill,                //����                    8
  DownHill,              //����                    9
  Triangle_Left,         //ǰ��ʶ����������ת������  13
  Triangle_Left_GoIn,    //���ǳ�����ǰ���󳵴�ʱ���������� 14
  Triangle_Left_Ing,     //15
  Triangle_Right,        //ǰ��ʶ����������ת������  16
  Triangle_Right_GoIn,   //���ǳ�����ǰ���󳵴�ʱ����������    17
  Triangle_Right_Ing,    //                                       18
  Overtake_Left,         //��ӿ�����                           19
  Overtake_Left_ing,     //�����󳬳���                           20
  Overtake_Left_Return,  //���󳬳�������                         21
  Overtake_Left_GoStraight,//�󳬳�����������ǰ����Ϊ���м�ģʽ 22
  Overtake_Left_Rerturn_Prepare,//�󳬳�������׼������         23
  Overtake_Left_Rerturn_Execute,//�󳬳������󳵿�ʼ����         24
  Overtake_Right,        //�Ҽӿ�����                           25
  Overtake_Right_ing,    //�����ҳ�����                           26
  Overtake_Right_Return, //���ҳ���������                         27
  Overtake_Right_GoStraight,//�ҳ�������������ǰ����Ϊ���м�ģʽ 28
  Overtake_Right_Rerturn_Prepare,//�ҳ���������׼������          29
  Overtake_Right_Rerturn_Execute,//�ҳ��������󳵿�ʼ����          30

}Road_Type;
extern Road_Type RoadType,RoadType_Last,RoadType_AnotherCar;
typedef enum //���ú���������
{
  NoError,          //0
  MiddleCorrect,    //1            
  MiddleCorrect2,
  LeftCorrect_1,    //2
  LeftCorrect_2,    //3
  LeftCorrect_3,    //4
  LeftCorrect_4,    //5
  LeftCorrect_5,    //6
  LeftCorrect_6,    //7
  RightCorrect_1,   //8
  RightCorrect_2,   //9
  RightCorrect_3,   //10
  RightCorrect_4,   //11
  RightCorrect_5,   //12
  RightCorrect_6,   //13
  Rod_L,
  Rod_R,
}Error_Type;
extern Error_Type ErrorType;
typedef enum
{
  Curve_Null,
  Straight,
  GoInBend,
  Bend,
}Curve_Type;
extern Curve_Type CurveType;
#define ImgWhite 33
#define ImgBlack 126
#define ImgGreen 2
#define ImgBlue  3
#define ImgRed   4
#define LostWhite 1
#define LostBlack 2
#define ROAD_HALF 20
#define ROAD_W    40

#define RowMax  73      //����ƫ��ʱ���õ������ܺ�
#define VarReceiveMax 25
#define VarSendMax    25
extern uint8 ImgBufferAgo[CAMERA_SIZE+360+(VarReceiveMax+VarSendMax)*3+10],ImgBufferNow[CAMERA_SIZE+360+(VarReceiveMax+VarSendMax)*3+10];//ԭʼѹ��ͼ��洢������
extern uint8 Img[CAMERA_H][CAMERA_W];           //��ѹ�����ͼ��
extern uint8 real_x[CAMERA_H][CAMERA_W],real_y[CAMERA_H][CAMERA_W];//��ʵ����
extern float Img_P[12];                        //ͼ��Ȩ�رȣ�ÿ10��һ��Ȩ�ر�
extern float a2,b2,c2,d2,e2,f2,v2;                //��У��ϵ��
extern uint8 *ImgAgo,*ImgNow;                  //�л�ָ��
extern uint8 Left[CAMERA_H],Middle[CAMERA_H],Right[CAMERA_H];//��,�У�����ֵ
extern int Last_L,Last_R;                    //��һ�еı߽�ֵ
extern uint8 Jizhun_L[3],Jizhun_R[3];          //��׼����
extern uint8 Lost_L[CAMERA_H],Lost_R[CAMERA_H];        //�������
extern int Left_Init,Right_Init,Middle_Init;           //�����ҳ�ʼֵ����ֹ����ʱ����Խ��
extern int Sight_Last,Sight,Sight_L,Sight_R,Sight_Init,Sight_L_Init,Sight_R_Init;//ǰհ����ʼ��
extern int Sight_Init_Turn;
extern uint8 Lost_L_Num,Lost_R_Num;    //�����ۼ�
extern uint8 ValidLine;                //��Ч����
extern uint32 ImgNum_GoInBend;         //����ͼƬ��¼
extern uint32 Img_GoOutBend;           //����ͼƬ��¼
extern uint32 Flag_GoInBend;           //ֱ�������־λ
extern uint32 Flag_GoOutBend;          //ֱ�߳����־λ
extern uint32 ImgNum;                  //ͼƬ��Ŀ
extern uint32 ImgStopNum;                      //ֹͣʱ��ͼƬID
extern uint32 StraightNum;             //����ֱ������Ŀ
extern uint32 BendNum;                 //�����������Ŀ
extern uint8  CrossNum;                //ʮ����Ŀ
extern float MiddleError,MiddleError_Last,MiddleError_Previous;              //����ͼ��ƫ��,�ϳ������ϳ�
extern float AbsMiddleError;           //ͼ��ƫ��ľ���ֵ
extern float ImgCurveDegree;           //���������̶�ϵ��

/******************���*****************/
extern int Dj_Mid,Dj_Left,Dj_Right;
extern float Duoji_P,Duoji_P_Cross,Duoji_D_Cross,Duoji_E,Duoji_D_A,Duoji_D,UP,UP_Max,UD;  //���P,D
extern float DP_A,DP_B,DP_C;   //�����̬P�Ĳ���
extern float Duoji_D_A;
extern float Kp_DuojiCorrect;
extern int PWM_Duoji;
extern uint8 Duoji_DividingLine;    //�������ߺͶ�ֵP�ķֽ���
/*****************�ٶȿ���*********************/
#define MotorCH1  FTM_CH3
#define MotorCH2  FTM_CH4
extern int SpeedMax;           //����ٶ�
extern int SpeedMid;           //�м��ٶ�
extern float SpeedChase_Range;   //׷���ٶȱ仯��Χ
extern int SpeedChase_KP;      //׷���ٶȱ仯ϵ��
extern int SpeedTriangle;      //���ǳ��������ٶ�
extern int SpeedMin;           //��С�ٶ�
extern int SpeedGoal;          //�趨�ٶ�
extern int Speed_P;            //�ٶ�P��
extern int Speed_I;            //�ٶ�I��
extern int Motor_P;            //���P����
extern int Motor_I;            //���I����
extern int MotorOutMax;        //���������PWM
extern int MotorOutMin;        //�����С���PWM
extern int16 SpeedNow;              //��ǰ�ٶ�
extern int MotorError[3];       //���ƫ��
extern int PWM_Motor,PWM_L,PWM_R; //���PWM
extern int CarTime;                    //��������һȦ��ʱ��
extern int SpeedAverage;               //һȦ������ƽ���ٶ�
extern int PID_Test_Time;              //�ٶ�PID���β�����ʱ��
extern float Distance,Distance_Last;              //·��,��λ������
extern float Distance_StartLine;        //������·�̼�¼
extern float Distance_Cross,Distance_Cross_GoOut;            //��ʮ��·�̼�¼
extern float Distance_Overtake;
extern float Distance_GoOutBend;        //����·�̼�¼
extern float Distance_GoInBend;
extern float Distance_Overtake_Right_GoStraight;
extern float Distance_Overtake_Left_GoStraight;
extern int Distance_TwoCar;                //˫������
extern int PWM_BangBangMax;          //bangbang�������
extern int PWM_BangBangMin;          //bangbang�������
extern int BangBang_Yuzhi;           //bangbang��ֵ
extern int Flag_BangBang;              //bangbang��־λ
extern long Count_Total,Count_Total_Last;               //�����ۼ�ֵ
extern uint8 Flag_Brake;               //ɲ����־λ
extern float Distance_Far;
extern float Distance_Near;
extern float Distance_Close;
extern float Distance_VeryFar; 
extern float Distance_Mid ;
extern int PWM_Overtake;
extern float Kv;
/*****************��*********************/
extern uint32  CarState;
extern uint32 Time_Duoji,Time_SD,Time_Camera,Time_Suanfa,Time_1,Time_2,Time_3,Time_4,Time_5;      //��Ӧ����ִ��ʱ��
extern float Voltage ;                                         //��ص�ѹ
extern uint8 CarFlag;
extern int VarSend[VarSendMax];
extern int VarReceive[VarReceiveMax];
extern uint8 FlagStart;
typedef enum          //ǰ�����Ǻ�
{
  Front,
  Later,
}Car_Type;
typedef enum          //�ٶȿ��Ʊ�־λ
{
  Speed_Null,
  Speed_Up,
  Brake,                //��תɲ��
  Reversal_Slight,     //��תɲ��,��΢
  Reversal_Urgency,     //��תɲ��������
}Speed_Flag;
typedef enum          //�Ƿ�����ͼ
{
  OPEN,
  OFF,
}IMG_TX_SELECT;
extern Car_Type CarType;
extern IMG_TX_SELECT IMG_TX;
extern Speed_Flag SpeedFlag,SpeedFlag_Last;
extern int Light_X ,Light_Y,Light_Bottom,Light_Left,Light_Right,Light_Top;   //ǰ����Դ���꣬������
extern uint8 Nrf_Tx[14];                                       //Nrf���ͻ�����
extern uint8 Nrf_Rx[14];                                       //Nrf���ջ�����

//��������״̬��־λ
/*****************SD��*********************/
extern uint8 SD_Var[(VarReceiveMax+VarSendMax)*3];
extern uint8 SD_Boundary[CAMERA_H*2*3];//������λ����javaд��java��byte���ͷ�ΧΪ-128~127�����ֱ���Ϊ160��������127������Ҫת��һ�¡�

/*****************flash*********************/
#define SECTOR_NUM  (FLASH_SECTOR_NUM-1)         //������������������ȷ����ȫ
/*****************����*********************/
extern uint8 Uart_Buffer[];
extern int Uart_Count;
#define VarExchangeMax 10
/*****************LCD*********************/
extern int16 page,LastPage;             //ҳ��
extern int16 Img_LCD_Loaction_X,Img_LCD_Loaction_Y;      //ͼ����LCD�ϵ�λ��
extern uint8 Flag_LCD_White;           //Һ��ˢ�ױ�־λ
#endif