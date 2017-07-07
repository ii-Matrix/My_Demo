#ifndef _OLDBOY_VAR_H_
#define _OLDBOY_VAR_H_
//说明：由于一些控制参数都是从flash读取的，所以那些参数定义时并没有给初值，额，赛前为了保险起见还是给个初值吧，万一那片flash被擦除的次数太多，坏了肿么办。

/******************摄像头及图像*****************/
typedef enum           //赛道类型
{
  Normal,                 //正常                    0
  GoOutBend,             //出弯                     1
  Cross,                 //正入十字                 2
  CrossL,                //斜入十字，向左转         3
  CrossR,                //斜入十字，向右转         4
  StartLine,             //起跑线                  5
  Obstacle_L,            //左障碍                  6
  Obstacle_R,             //右障碍                 7
  UpHill,                //上坡                    8
  DownHill,              //下坡                    9
  Triangle_Left,         //前车识别到三角形左转超车区  13
  Triangle_Left_GoIn,    //三角超车区前，后车此时补弯曲的线 14
  Triangle_Left_Ing,     //15
  Triangle_Right,        //前车识别到三角形右转超车区  16
  Triangle_Right_GoIn,   //三角超车区前，后车此时补弯曲的线    17
  Triangle_Right_Ing,    //                                       18
  Overtake_Left,         //左加宽超车区                           19
  Overtake_Left_ing,     //进入左超车区                           20
  Overtake_Left_Return,  //从左超车区返回                         21
  Overtake_Left_GoStraight,//左超车区，超车后，前车变为走中间模式 22
  Overtake_Left_Rerturn_Prepare,//左超车区，后车准备返回         23
  Overtake_Left_Rerturn_Execute,//左超车区，后车开始返回         24
  Overtake_Right,        //右加宽超车区                           25
  Overtake_Right_ing,    //进入右超车区                           26
  Overtake_Right_Return, //从右超车区返回                         27
  Overtake_Right_GoStraight,//右超车区，超车后，前车变为走中间模式 28
  Overtake_Right_Rerturn_Prepare,//右超车区，后车准备返回          29
  Overtake_Right_Rerturn_Execute,//右超车区，后车开始返回          30

}Road_Type;
extern Road_Type RoadType,RoadType_Last,RoadType_AnotherCar;
typedef enum //调用函数的名称
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

#define RowMax  73      //计算偏差时所用的行数总和
#define VarReceiveMax 25
#define VarSendMax    25
extern uint8 ImgBufferAgo[CAMERA_SIZE+360+(VarReceiveMax+VarSendMax)*3+10],ImgBufferNow[CAMERA_SIZE+360+(VarReceiveMax+VarSendMax)*3+10];//原始压缩图像存储缓冲区
extern uint8 Img[CAMERA_H][CAMERA_W];           //解压缩后的图像
extern uint8 real_x[CAMERA_H][CAMERA_W],real_y[CAMERA_H][CAMERA_W];//真实坐标
extern float Img_P[12];                        //图像权重比，每10行一个权重比
extern float a2,b2,c2,d2,e2,f2,v2;                //反校正系数
extern uint8 *ImgAgo,*ImgNow;                  //切换指针
extern uint8 Left[CAMERA_H],Middle[CAMERA_H],Right[CAMERA_H];//左,中，右线值
extern int Last_L,Last_R;                    //上一行的边界值
extern uint8 Jizhun_L[3],Jizhun_R[3];          //基准数组
extern uint8 Lost_L[CAMERA_H],Lost_R[CAMERA_H];        //丢线情况
extern int Left_Init,Right_Init,Middle_Init;           //左中右初始值，防止搜线时数组越界
extern int Sight_Last,Sight,Sight_L,Sight_R,Sight_Init,Sight_L_Init,Sight_R_Init;//前瞻及初始化
extern int Sight_Init_Turn;
extern uint8 Lost_L_Num,Lost_R_Num;    //丢线累计
extern uint8 ValidLine;                //有效行数
extern uint32 ImgNum_GoInBend;         //入弯图片记录
extern uint32 Img_GoOutBend;           //出弯图片记录
extern uint32 Flag_GoInBend;           //直线入弯标志位
extern uint32 Flag_GoOutBend;          //直线出弯标志位
extern uint32 ImgNum;                  //图片数目
extern uint32 ImgStopNum;                      //停止时的图片ID
extern uint32 StraightNum;             //连续直道的数目
extern uint32 BendNum;                 //连续弯道的数目
extern uint8  CrossNum;                //十字数目
extern float MiddleError,MiddleError_Last,MiddleError_Previous;              //本场图像偏差,上场，上上场
extern float AbsMiddleError;           //图像偏差的绝对值
extern float ImgCurveDegree;           //中线弯曲程度系数

/******************舵机*****************/
extern int Dj_Mid,Dj_Left,Dj_Right;
extern float Duoji_P,Duoji_P_Cross,Duoji_D_Cross,Duoji_E,Duoji_D_A,Duoji_D,UP,UP_Max,UD;  //舵机P,D
extern float DP_A,DP_B,DP_C;   //舵机动态P的参数
extern float Duoji_D_A;
extern float Kp_DuojiCorrect;
extern int PWM_Duoji;
extern uint8 Duoji_DividingLine;    //二次曲线和定值P的分界线
/*****************速度控制*********************/
#define MotorCH1  FTM_CH3
#define MotorCH2  FTM_CH4
extern int SpeedMax;           //最大速度
extern int SpeedMid;           //中间速度
extern float SpeedChase_Range;   //追逐速度变化范围
extern int SpeedChase_KP;      //追逐速度变化系数
extern int SpeedTriangle;      //三角超车区后车速度
extern int SpeedMin;           //最小速度
extern int SpeedGoal;          //设定速度
extern int Speed_P;            //速度P项
extern int Speed_I;            //速度I项
extern int Motor_P;            //电机P参数
extern int Motor_I;            //电机I参数
extern int MotorOutMax;        //电机最大输出PWM
extern int MotorOutMin;        //电机最小输出PWM
extern int16 SpeedNow;              //当前速度
extern int MotorError[3];       //电机偏差
extern int PWM_Motor,PWM_L,PWM_R; //电机PWM
extern int CarTime;                    //车子运行一圈的时间
extern int SpeedAverage;               //一圈结束后平均速度
extern int PID_Test_Time;              //速度PID矩形波测试时间
extern float Distance,Distance_Last;              //路程,单位：厘米
extern float Distance_StartLine;        //起跑线路程记录
extern float Distance_Cross,Distance_Cross_GoOut;            //入十字路程记录
extern float Distance_Overtake;
extern float Distance_GoOutBend;        //出弯路程记录
extern float Distance_GoInBend;
extern float Distance_Overtake_Right_GoStraight;
extern float Distance_Overtake_Left_GoStraight;
extern int Distance_TwoCar;                //双车距离
extern int PWM_BangBangMax;          //bangbang输出上限
extern int PWM_BangBangMin;          //bangbang输出下限
extern int BangBang_Yuzhi;           //bangbang阈值
extern int Flag_BangBang;              //bangbang标志位
extern long Count_Total,Count_Total_Last;               //脉冲累计值
extern uint8 Flag_Brake;               //刹车标志位
extern float Distance_Far;
extern float Distance_Near;
extern float Distance_Close;
extern float Distance_VeryFar; 
extern float Distance_Mid ;
extern int PWM_Overtake;
extern float Kv;
/*****************车*********************/
extern uint32  CarState;
extern uint32 Time_Duoji,Time_SD,Time_Camera,Time_Suanfa,Time_1,Time_2,Time_3,Time_4,Time_5;      //相应函数执行时间
extern float Voltage ;                                         //电池电压
extern uint8 CarFlag;
extern int VarSend[VarSendMax];
extern int VarReceive[VarReceiveMax];
extern uint8 FlagStart;
typedef enum          //前车还是后车
{
  Front,
  Later,
}Car_Type;
typedef enum          //速度控制标志位
{
  Speed_Null,
  Speed_Up,
  Brake,                //空转刹车
  Reversal_Slight,     //反转刹车,轻微
  Reversal_Urgency,     //反转刹车，紧急
}Speed_Flag;
typedef enum          //是否开启传图
{
  OPEN,
  OFF,
}IMG_TX_SELECT;
extern Car_Type CarType;
extern IMG_TX_SELECT IMG_TX;
extern Speed_Flag SpeedFlag,SpeedFlag_Last;
extern int Light_X ,Light_Y,Light_Bottom,Light_Left,Light_Right,Light_Top;   //前车光源坐标，半宽，半高
extern uint8 Nrf_Tx[14];                                       //Nrf发送缓冲区
extern uint8 Nrf_Rx[14];                                       //Nrf接收缓冲区

//车子运行状态标志位
/*****************SD卡*********************/
extern uint8 SD_Var[(VarReceiveMax+VarSendMax)*3];
extern uint8 SD_Boundary[CAMERA_H*2*3];//由于上位机用java写，java中byte类型范围为-128~127，而分辨率为160，超过了127，所以要转换一下。

/*****************flash*********************/
#define SECTOR_NUM  (FLASH_SECTOR_NUM-1)         //尽量用最后面的扇区，确保安全
/*****************串口*********************/
extern uint8 Uart_Buffer[];
extern int Uart_Count;
#define VarExchangeMax 10
/*****************LCD*********************/
extern int16 page,LastPage;             //页数
extern int16 Img_LCD_Loaction_X,Img_LCD_Loaction_Y;      //图像在LCD上的位置
extern uint8 Flag_LCD_White;           //液晶刷白标志位
#endif