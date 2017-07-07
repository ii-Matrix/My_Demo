/***********************************************************
老男孩之                      图像处理
***********************************************************/
#include "common.h"
#include "include.h"
#include  "Var.h"
#include  "ImageDeal.h"
#define CurveTypeRecordMax 20
uint8 CurveType_History[CurveTypeRecordMax];
uint8 ImgBufferAgo[CAMERA_SIZE+360+(VarReceiveMax+VarSendMax)*3+10],ImgBufferNow[CAMERA_SIZE+360+(VarReceiveMax+VarSendMax)*3+10];//原始压缩图像存储缓冲区
uint8 Img[CAMERA_H][CAMERA_W];           //解压缩后的图像
uint8 *ImgNow,*ImgAgo;
//uint8 SelectRow[RowMax];              //计算偏差时所用的行，等距离分割，解决近处图像比例过大的问题
//uint8 SelectRowFlag[CAMERA_H];          //计算偏差所用行标志位，1：代表参与运算
float Img_P[12];                        //图像权重比，每10行一个权重比
float a2,b2,c2,d2,e2,f2,v2;                //反校正系数
uint8 Left[CAMERA_H],Middle[CAMERA_H],Right[CAMERA_H];//左,中，右线值
int Last_L,Last_R;                    //上一行的边界值
uint8 Jizhun_L[3],Jizhun_R[3];          //基准数组
uint8 Lost_L[CAMERA_H],Lost_R[CAMERA_H];        //丢线情况
//int Left_Init=3,Right_Init=156,Middle_Init=80;           //左中右初始值，防止搜线时数组越界
int Left_Init=3,Right_Init=146,Middle_Init=69;           //左中右初始值，防止搜线时数组越界
int Sight_Last,Sight=119,Sight_L=24,Sight_R=24,Sight_Init=24,Sight_L_Init=24,Sight_R_Init=24;//前瞻及初始化
int Sight_Init_Turn  = 26;
Road_Type RoadType,RoadType_Last,RoadType_AnotherCar;                 //赛道类型
uint8 Lost_L_Num,Lost_R_Num;    //丢线累计
uint8 ValidLine;                //有效行数
uint32 ImgNum_GoInBend;         //入弯图片记录
uint32 Img_GoOutBend;           //出弯图片记录
uint32 Flag_GoInBend;           //直线入弯标志位
uint32 Flag_GoOutBend;          //直线出弯标志位
uint32 ImgNum;                  //图片数目
uint32 ImgStopNum;              //停止时的图片ID
uint32 StraightNum;             //连续直道的数目
uint32 BendNum;                 //连续弯道的数目
uint32 Img_GoOutBend;            //出弯时的图片数目
uint8  CrossNum;                //十字数目
uint8  LightLimtY_BottomFind = 78;  //后车底部寻找时，前车光源坐标大于这个值时，采用另一种方案寻找
int Light_X ,Light_Y,Light_Bottom,Light_Top,Light_Left,Light_Right;          //前车光源坐标，半宽,半高                                         //前车半高
float MiddleError,MiddleError_Last,MiddleError_Previous;              //本场图像偏差,上场，上上场
float AbsMiddleError;           //图像偏差的绝对值
float ImgCurveDegree;           //中线弯曲程度系数
float ;           //入十字记录
Error_Type ErrorType;           //图像出错原因记录
Curve_Type CurveType;           //弯曲程度，主要为了速度控制服务
int Row;
float Cross_Xielv,Cross_Xielv2;
int Y_Stop_GenZongAgain;
uint8 Y_Overtake_Start;
float XieLv_Overtake_Middle,XieLv_Overtake_Left,XieLv_Overtake_Right;
int Road_OvetakeFind_X;
/****************************
作用：图像底部寻找，确定基准值
****************************/
void BottomFind()
{   
  int x=0,y=0;                          //y为纵坐标，x为横坐标
  int BlackNum_L=0,BlackNum_R=0;        //底部三行左右黑点总数
  int Start_L = 80;
  int Start_R = 80 ;                      //底部搜索起始点
  if(CarType == Front)                  //前车寻找
  {
    for(y=119;y>116;y--)                  //统计左右黑点数目，若左>右，从稍靠右开始搜索
    {
      for(int x = 79;x>4;x-=4)            //-4是为了加快速度，大致判断没必要做精确
      {
        if(Img[y][x]==ImgBlack)
          BlackNum_L++;
      }
      for(int x = 155;x>80;x-=4)
      {
        if(Img[y][x]==ImgBlack)
          BlackNum_R++;
      }
    }
    if(BlackNum_R>BlackNum_L)
      Start_L = Start_R = 52;
    else if(BlackNum_R<BlackNum_L)
      Start_L = Start_R = 108;
    //    if(RoadType == Triangle_Left || RoadType == Triangle_Right)
    //    {
    //      
    //    }
    for(y=119;y>116;y--)
    {   
      //找右基准
      for(x=Start_R;x<Right_Init;x++)
      {   //寻找跳变点
        if(Img[y][x+1]==ImgBlack && Img[y][x+2]==ImgBlack && Img[y][x]==ImgWhite && Img[y][x-1]==ImgWhite && Img[y][x-2]==ImgWhite && Img[y][x-3]==ImgWhite)
        {
          Right[y]=x-1;             //记录
          Lost_R[y]=0;            //丢线标志位清零
          break;
        }
      }
      
      //找左基准
      for(x=Start_L;x>Left_Init;x--)
      {
        //寻找跳变点
        if(Img[y][x-1]==ImgBlack && Img[y][x-2]==ImgBlack && Img[y][x]==ImgWhite && Img[y][x+1]==ImgWhite && Img[y][x+2]==ImgWhite && Img[y][x+3]==ImgWhite)
        {
          Left[y]=x+1;             //记录
          Lost_L[y]=0;            //丢线标志位清零
          break;
        }
      }
      Jizhun_R[119-y]=Right[y];
      Jizhun_L[119-y]=Left[y];
      Middle[y]=(Right[y]+Left[y])/2;
      //   lvbo_Jizhun(y);
    }                       
    //计算基准均值
    Last_L=(Jizhun_L[0]+Jizhun_L[1]+Jizhun_L[2])/3;
    Last_R=(Jizhun_R[0]+Jizhun_R[1]+Jizhun_R[2])/3;
  }
  if(CarType == Later)                          //后车寻找
  {
    
    //    else
    //      CarType = Front;
    if(Light_Y<LightLimtY_BottomFind)
    {
      for(y=119;y>116;y--)                  //统计左右黑点数目，若左>右，从稍靠右开始搜索
      {
        for(int x = 79;x>4;x-=4)            //-4是为了加快速度，大致判断没必要做精确
        {
          if(Img[y][x]==ImgBlack)
            BlackNum_L++;
        }
        for(int x = 155;x>80;x-=4)
        {
          if(Img[y][x]==ImgBlack)
            BlackNum_R++;
        }
      }
      if(BlackNum_R>BlackNum_L)
        Start_L = Start_R = 52;
      else if(BlackNum_R<BlackNum_L)
        Start_L = Start_R = 108;
    }
    else
    {
      if(Light_X>80)
      {
        Start_L = Light_X-29;
        Start_R = Light_X+25;
      }
      else
      {
        Start_L = Light_X-25;
        Start_R = Light_X+29;      
      }
    }
    for(y=119;y>116;y--)
    {   
      //找右基准
      for(x=Start_R;x<Right_Init;x++)
      {   //寻找跳变点
        if(Img[y][x]==ImgBlack && Img[y][x+1]==ImgBlack && Img[y][x-1]==ImgWhite && Img[y][x-2]==ImgWhite)
        {
          if(Img[y][x-12] == ImgWhite) // 两车过近时可能将后轮内测找成边界
          {
            Right[y]=x-1;             //记录
            Lost_R[y]=0;            //丢线标志位清零
            break;
          }
        }
      }
      
      //找左基准
      for(x=Start_L;x>Left_Init;x--)
      {
        //寻找跳变点
        if(Img[y][x]==ImgBlack && Img[y][x-1]==ImgBlack && Img[y][x+1]==ImgWhite && Img[y][x+2]==ImgWhite)
        {
          if(Img[y][x+12] == ImgWhite) // 两车过近时可能将后轮内测找成边界
          {
            Left[y]=x+1;             //记录
            Lost_L[y]=0;            //丢线标志位清零
            break;
          }
        }
      }
      if(y<119 && Light_Y>=LightLimtY_BottomFind)
      {//两车太近的情况
        int YuZhi = 10;
        if(abs_sub(Right[y],Right[y+1])>YuZhi)
          Right[y]=Right[y+1];
        if(abs_sub(Left[y],Left[y+1])>YuZhi)
          Left[y]=Left[y+1];
      }
      // if(0)
      if(Light_Y>=LightLimtY_BottomFind)
      {
        if(Left[y] == Left_Init && Img[y][Left[y]] == ImgBlack)
        {
          for(uint8 x = Left[y];x<80;x++)
          {
            if(Img[y][x] == ImgWhite && Img[y][x+1] == ImgWhite && Img[y][x-1] == ImgBlack && Img[y][x-2] == ImgBlack)
            {
              for(uint8 x2 = x;x2<80;x2++)
              {
                if(Img[y][x2] == ImgBlack && Img[y][x2+1] == ImgBlack && Img[y][x2-1] == ImgWhite && Img[y][x2-2] == ImgWhite)
                {
                  if(abs_sub(x2,x)<20 && (y<119 && Img[y+1][x] == ImgBlack))
                  {
                    Left[y] = x;
                    x = 80;           //跳出循环
                    break;
                  }
                }
              }
            }
          }
        }
        if(Right[y] == Right_Init && Img[y][Right[y]] == ImgBlack)
        {
          for(uint8 x = Right[y];x>80;x--)
          {
            if(Img[y][x] == ImgWhite && Img[y][x-1] == ImgWhite && Img[y][x+1] == ImgBlack && Img[y][x+2] == ImgBlack)
            {
              for(uint8 x2 = x;x2>80;x2--)
              {
                if(Img[y][x2] == ImgBlack && Img[y][x2-1] == ImgBlack && Img[y][x2+1] == ImgWhite && Img[y][x2+2] == ImgWhite)
                {
                  if(abs_sub(x2,x)<20 && (y<119 && Img[y+1][x] == ImgBlack))
                  {
                    Right[y] = x;
                    x = 80;           //跳出循环
                    break;
                  }
                }
              }
            }
          }
        }
      }
      Jizhun_R[119-y]=Right[y];
      Jizhun_L[119-y]=Left[y];
      Middle[y]=(Right[y]+Left[y])/2;
      
      //   lvbo_Jizhun(y);
    }
    //计算基准均值
    Last_L=Left[117];
    Last_R=Right[117]; 
  }
}


/****************************
作用：找到边界线
****************************/
void GenZong()
{
  int x=0,y=0;
  int Window = 10;
  if(CarType == Front)                  //前车找边界
  {
    uint8 Final_L=Left_Init,Final_R=Right_Init,Start_L,Start_R;
    
    //一路跟踪到有效前瞻，嘎嘎
    for(y=116;y>=Sight_Init;y--)// && Sight_L==Sight_L_Init && Sight_R==Sight_R_Init;y--)//判断项这么长是因为有时候要改变Sight之类的值，结束搜索
    {
      // if(Sight_L==Sight_L_Init)
      {
        //左边界从右向左搜 
        if(Last_L-Window>Left_Init)
          Final_L=Last_L-Window;       
        if(Last_L+Window<Right_Init)
          Start_L=Last_L+Window;
        else
          Start_L=Right_Init;
        for(x=Start_L;x>Final_L;x--)     
        {
          if(Img[y][x]==ImgBlack)//找到黑点
          {//进一步确认
            if(Img[y][x-1]==ImgBlack && Img[y][x-2]==ImgBlack && Img[y][x+1]==ImgWhite && Img[y][x+2]==ImgWhite)
            { 
              Left[y]=x+2;
              Lost_L[y]=0;        //丢线标志位清零
              break;        //退出循环 
            }
          }           
        }
        Last_L=Left[y];     //保存上一次的值
        if(Left[y]==Left_Init)//说明没有提取到边界
        {
          Last_L=Left[y]=Refind_L(y);     //再次寻找
        }
      }
      //   if(Sight_R==Sight_R_Init)
      {
        if(Last_R+Window<Right_Init)
          Final_R=Last_R+Window;
        else
          Final_R=Right_Init;
        if(Last_R-Window>Left_Init)
          Start_R=Last_R-Window;
        else
          Start_R=Left_Init;
        //右边界从左向右搜 
        for(x=Start_R;x<Final_R;x++) 
        {
          if(Img[y][x]==ImgBlack)//找到黑点
          {//进一步确认
            if(Img[y][x+1]==ImgBlack && Img[y][x+2]==ImgBlack && Img[y][x-1]==ImgWhite && Img[y][x-2]==ImgWhite)
            { 
              Right[y]=x-2;
              Lost_R[y]=0;
              break;//退出循环
            }
          }           
        }
        Last_R=Right[y];     //保存上一次的值
        if(Right[y]==Right_Init)//说明右边界没有提取到
        {
          Last_R=Right[y]=Refind_R(y);    
        }
      }
    }
  }

}
/****************************
作用：计算中线
****************************/
void jisuan_Middle()
{
  int temp_x=0,temp_y=0,temp_M=0;
  uint8 flag=0;
  //int middle=80;
  int middle=69;
  
  //for(int y=116;y>=Sight_Init_Turn;y--)
  for(int y=116;y>=10;y--)
  {
    //if(CarType == Front)
    {
      //    Middle[y] = (Left[y]+Right[y])/2;
      if(Lost_L[y]==0 && Lost_R[y]==0)      //两边都能找到
      {
        middle=(Left[y]+Right[y])/2;  
      }
      else if((0!=Lost_L[y]) && (0!=Lost_R[y]))     //两边都没有
      {
        if((Lost_L[y]==LostWhite) && (Lost_R[y]==LostWhite))     //两边都没有,并且丢白线，可能是十字
        {
          middle=(Middle[y+1]+Middle[y+2]+Middle[y+3])/3;       //中点等于前三行均值,计算偏差时本行不参与
          //  if(CarState == CarStop)//太耗时，会打乱时间
          {
            Last_L=Left[y]=fanjiaozheng_x(y,middle,-ROAD_HALF);   //平移赛道宽度一半补边界线
            Last_R=Right[y]=fanjiaozheng_x(y,middle,+ROAD_HALF);
          }       
        }
        else
          middle=(Middle[y+3]+Middle[y+2]+Middle[y+1])/3;
      }
      else if((0==Lost_L[y]) && (0!=Lost_R[y]))     //左边有，右边没有
      { 
        Lost_R_Num++;
        if(Lost_R[y+1] ==LostWhite && Lost_L[y+1] == LostWhite)
          middle = Middle[y+1];
        else       
          middle=Middle[y+1]+(Left[y]-Left[y+1]);//+(int)(Lost_R_Num*Lost_R_Num*ImgCurveDegree);  //按照左边变化趋势补线
      }
      
      else if((0!=Lost_L[y]) && (0==Lost_R[y]))     //左边没有，右边有
      { 
        Lost_L_Num++;
        if(Lost_R[y+1] ==LostWhite && Lost_L[y+1] == LostWhite)
          middle = Middle[y+1];
        else
          middle=Middle[y+1]-(Right[y+1]-Right[y]);//-(int)(Lost_L_Num*Lost_L_Num*ImgCurveDegree);  //按照右边变化趋势补线   
      }
    }

    if(middle>Right_Init)
      middle=Right_Init;
    if(middle<Left_Init)
      middle=Left_Init;
    Middle[y]=middle;
  }
  //  Lvbo_Middle();//缝合
  
  
}
/****************************
作用：未找到边界线时再次寻找
参数：当前行的纵坐标
返回：找到的边界线横坐标
****************************/
uint8 Refind_L(uint8 y)//再找一次左边界
{
  int Final,Start,x;            //。。。又吃了一次数据类型的亏，不能是uint8，负数变正后可能比159大
  if(CarType == Front)
  {
    if(RoadType!=Overtake_Right_ing && RoadType!= Overtake_Left_ing)
    {
    uint8 Final_L=Left_Init,Final_R=Right_Init,Start_L,Start_R;
    int window = 22;
    //左边界从右向左搜 
    if((int)Left[y+1]-window>Left_Init)
      Final_L=Left[y+1]-window;       
    if(Left[y+1]+window<Right_Init)
      Start_L=Left[y+1]+window;
    else
      Start_L=Right_Init;
    for(x=Start_L;x>Final_L;x--)     
    {
      if(Img[y][x]==ImgBlack)//找到黑点
      {//进一步确认
        if(Img[y][x-1]==ImgBlack && Img[y][x-2]==ImgBlack && Img[y][x+1]==ImgWhite && Img[y][x+2]==ImgWhite)
        { 
          Lost_L[y]=0;        //丢线标志位清零
          return x+2;  
        }
      } }          
    }
    //依旧找不到，启动中心扩散搜索
    if(Left[y+1]<78 && Right[y+1]>82  && RoadType!=Overtake_Right_ing && RoadType!= Overtake_Left_ing)      //中心扩散搜索，多用于正入十字
    {
      for(x=80;x>Left_Init;x--)     
      {
        if(Img[y][x]==ImgBlack)//找到黑点
        {//进一步确认
          if(Img[y][x-1]==ImgBlack && Img[y][x-2]==ImgBlack && Img[y][x+1]==ImgWhite && Img[y][x+2]==ImgWhite && Img[y][x+3]==ImgWhite)
          { 
            Lost_L[y]=0;        //丢线标志位清零
            return x+2;            
          }           
        }
      }
    } 
    if(Img[y][Left[y+1]]==ImgWhite )//本行是白色                          
      Lost_L[y]=LostWhite;
    if(Img[y][Left[y+1]]==ImgBlack)//本行是黑色
      Lost_L[y]=LostBlack;
    return Left[y+1];//还是没找到，返回上一行的值
  }

}
/****************************
作用：未找到边界线时再次寻找
参数：当前行的纵坐标
返回：找到的边界线横坐标
****************************/
uint8 Refind_R(uint8 y)         //再找一次右边界
{
  int Final_R=Right_Init,Start_R;
  uint8 Final,Start,x; 
  if(CarType == Front )
  {
    if(RoadType!=Overtake_Right_ing && RoadType!= Overtake_Left_ing)
    {
    int window = 22;
    if(Right[y+1]+window<Right_Init)
      Final_R=Right[y+1]+window;
    else
      Final_R=Right_Init;
    if((int)Right[y+1]-window>Left_Init)
      Start_R=Right[y+1]-window;
    else
      Start_R=Left_Init;
    //右边界从左向右搜 
    for(x=Start_R;x<Final_R;x++) 
    {
      if(Img[y][x]==ImgBlack)//找到黑点
      {//进一步确认
        if(Img[y][x+1]==ImgBlack && Img[y][x+2]==ImgBlack && Img[y][x-1]==ImgWhite && Img[y][x-2]==ImgWhite)
        { 
          Lost_R[y]=0;        //丢线标志位清零
          return x-2; 
        }
      }           
    }
    }
    if(Left[y+1]<78 && Right[y+1]>82  && RoadType!=Overtake_Right_ing && RoadType!= Overtake_Left_ing)  
      //依旧找不到，启动中心扩散搜索
    {
      for(int x=80;x<Right_Init;x++)
      {
        if(Img[y][x]==ImgBlack)//找到黑点
        {//进一步确认
          if(Img[y][x+1]==ImgBlack && Img[y][x+2]==ImgBlack && Img[y][x-1]==ImgWhite && Img[y][x-2]==ImgWhite && Img[y][x-3]==ImgWhite)
          { 
            Lost_R[y]=0;        //丢线标志位清零
            return x-2;
          }
        }  
      }    
    }
    if(Img[y][Right[y+1]]==ImgWhite)                                   
      Lost_R[y]=LostWhite;
    
    if(Img[y][Right[y+1]]==ImgBlack)
      Lost_R[y]=LostBlack;
    return Right[y+1];
  }  

  if(Img[y][Right[y+1]]==ImgWhite)                                   
    Lost_R[y]=LostWhite;
  if(Img[y][Right[y+1]]==ImgBlack)
    Lost_R[y]=LostBlack;
  return Right[y+1];
}

/****************************
作用：动态前瞻的确定
****************************/
void ForeSight()
{
  if(Sight_L==Sight_L_Init)
  {
    for(int y=80;y>=Sight_Init;y--)      
    {  //纵向连续三点丢失，且都是黑色，则判断是赛道外
      // if(Img[y][Left[y]] == ImgBlack && Img[y-1][Left[y-1]] == ImgBlack && Img[y-2][Left[y-2]] == ImgBlack)
      if(Lost_L[y] == LostBlack && Lost_L[y-1] == LostBlack && Lost_L[y-2] == LostBlack)
      {
        Sight_L=y+1;
        break;
      }
      //左边界线比较靠右
      if(y<80 && Left[y]>150 && Left[y+1]>140 && Left[y]>Left[y+1] && Sight_L==Sight_L_Init)   
      {
        Sight_L=y;
        break;
      }
    }
  }
  
  if(Sight_R==Sight_R_Init)
  {
    for(int y=80;y>=Sight_Init;y--)
    {   //纵向连续三点丢失，且都是黑色，则判断是赛道外
      //   if(Img[y][Right[y]] == ImgBlack && Img[y-1][Right[y-1]] == ImgBlack && Img[y-2][Right[y-2]] == ImgBlack)
      if(Lost_R[y] == LostBlack && Lost_R[y-1] == LostBlack && Lost_R[y-2] == LostBlack)
      { 
        Sight_R=y+1;
        break;    
      }
      //      右边界线比较靠左
      if(y<80 && Right[y]<10 && Right[y+1]<20 && Right[y]<Right[y+1] && Sight_R==Sight_R_Init)   
      {
        Sight_R=y;
        break;
      }    
    }
  }
//  if(CarType == Later)
//  {
//    //去除杆的影响，4.24.16.2448
//    int YuZhi = 10;
//    for(int y = Light_Top-22;y>Sight_Init;y--)
//    {
//      if(abs_sub(Left[y],Right[y]<20) && Lost_L[y] == 0 && Lost_R[y] == 0)
//      {
//        Sight = Sight_L = Sight_R = y+3;
//        break;
//      }  
//    }
//  }
  if(Sight_L>Sight_R)   // 前瞻取最远的
    Sight=Sight_R;
  else
    Sight=Sight_L;
  if(RoadType == Overtake_Left_ing)
    Sight = Sight_L;
  if(RoadType == Overtake_Right_ing)
    Sight = Sight_R;  
}
/****************************
作用：判断赛道类型
****************************/
void judge_road()
{
  uint8 judge=0;        //引入这个变量的目的主要是判断出一个赛道类型后不再进行其他赛道类型的判断，减少误判
 // judge = OvertakeAreaFind(judge);    //加宽超车区识别
  //judge = OvertakeArea_ing(judge);    //进入超车区
  //  Overtake_LaterToFront();//后车变前车
  //  Overtake_FrontReturn(); //前车进入正常赛道
  //  Overtake_LaterReturn();               //后车从超车区返回
  //  OvertakeArea_Over();                  //结束超车
    //Overtake_Middle();
  //  Overtake_Return_GenZong();
  
  //和超车区冲突，要改！！
  CrossCheck();                      //入十字检测
  judge = judge_crossxieL(judge);    //左斜出十字          
  judge = judge_crossxieR(judge);    //右斜出十字
  
  StraightCheck();                  //直线检测
  
  GoInBendCheck();                  //入弯检测
  //OutsideCheck();                   //出界检测
  
}
/****************************
作用：右斜出十字判断及补线
参数：是否需要判断
返回：判断结果
****************************/
int  judge_crossxieR(uint8 judge)
{
  uint8 volatile flag_guai=0,y=0,a=0;
  int Distance_YuZhi = 400;
 // if(judge==0 && (RoadType == 0 || RoadType == Cross) && (Distance_Cross != 0 && Distance-Distance_Cross < Distance_YuZhi))
  if(judge==0 && (RoadType == 0 || RoadType == Cross))
  {
    float xielv,xielv2,xielv3;
    for(y=110;y>=Sight;y--)
    {
      //右边连续丢白色的同时，左边连续不丢失
      if(Lost_L[y]==0 && Lost_L[y+1]==0 && Lost_R[y]==LostWhite && Lost_R[y+1]==LostWhite)
        a++;
      else
        a=0;
      //左边界开始向左走，拐点之后
      if(a>5 && Left[y]>Left[y-1] && Left[y-1]>Left[y-2] && Left[y-2]>Left[y-3] && Left[y-3]>Left[y-4] && Left[y-4]>Left[y-5])
      {
        Cross_Xielv=jisuan_xielv_orginal(Left,y,y-10);
        Cross_Xielv2=jisuan_xielv_orginal(Left,y+10,y+1);
        if(Cross_Xielv>0.05  && Cross_Xielv2<0 && Cross_Xielv2>-5 )//斜率跳变，可能是斜入十字，可能是向左拐
        { 
          //          uint8 n=159-Left[y];
          //          uint8 WhitePoint=0,i;
          //          uint8 x=Left[y];
          //          for(i=0;i<n;i++)            //统计右方白点数目，加入全白限制    
          //          {
          //            if(Img[y][x+i]==ImgWhite) //唔，不知道如果再换为100度镜头后，右方是否为全白
          //              WhitePoint++;
          //          }
          //防误判
          if(Lost_R[y-1]==LostWhite && Lost_R[y-2]==LostWhite)
          {                                               //和正常弯道区别开
            flag_guai=1;
            break;
          }
          
        }
      }
    }    
    if(flag_guai==1)
    {
      flag_guai=0;
      int j;
      int temp_x,temp_y;
      RoadType=CrossR;     
      float xielv3=10/(Left[y+10]-Left[y]);    
      for(j=y+9;j>Sight_Init;j--)
      {
        int left=(j-y-10)*Cross_Xielv2+Left[y+10];               //按照斜率补线
        if(left>Right_Init)
          left = Right_Init;
        Left[j] = left;
        Middle[j]=fanjiaozheng_x(j,Left[j],ROAD_HALF);  //反校正补线
        Right[j]=fanjiaozheng_x(j,Left[j],ROAD_W);
        if(Middle[j]>Right_Init)                        //防止补过了，越界了
          Middle[j]=Right_Init;
        if(Right[j]>Right_Init)
          Right[j]=Right_Init;
        Lost_L[j]=Lost_R[j]=0;
      }
      Sight_L=Sight_R=Sight_Init;
      return 1;      
    }
  }
  return 0;
}
/****************************
作用：左斜出十字判断及补线
参数：是否需要判断
返回：判断结果
****************************/
int judge_crossxieL(uint8 judge)
{
  uint8 volatile flag_guai=0,y=0,a=0;
  int Distance_YuZhi = 400;//入十字3米内保持斜出十字判断
  //if(judge==0 && (RoadType == 0 || RoadType == Cross) && (Distance_Cross != 0 && Distance-Distance_Cross < Distance_YuZhi))
  if(judge==0 && (RoadType == 0 || RoadType == Cross))
  {
    float xielv,xielv2;
    for(Row=108;Row>=Sight;Row--)
    {
      //左边连续丢失的同时右边连续不丢失
      if(Lost_R[Row]==0  && Lost_R[Row+1]==0 && Lost_L[Row]==LostWhite && Lost_L[Row+1]==LostWhite)
        a++;
      else
        a=0;
      //右边界开始向右走，拐点之后
      if(a>5 && Right[Row]<Right[Row-1]&& Right[Row-1]<Right[Row-2] && Right[Row-2]<Right[Row-3] && Right[Row-3]<Right[Row-4]&& Right[Row-4]<Right[Row-5])
      {
        //绝对值越大越倾斜
        //        Cross_Xielv  = ((float)(int)Right[Row-10]-(int)Right[Row])/10.0;
        //        Cross_Xielv2 = ((float)(int)Right[Row+11]-(int)Right[Row+1])/10.0;
        //         if(Cross_Xielv>0.9    && Cross_Xielv2>0.8)
        Cross_Xielv=jisuan_xielv_orginal(Right,Row,Row-10);
        Cross_Xielv2=jisuan_xielv_orginal(Right,Row+10,Row+1);
        if(Cross_Xielv<-0.05 && Cross_Xielv2>0 && Cross_Xielv2<5)
        {
          //          uint8 n=Right[Row];
          //          uint8 WhitePoint=0,i;
          //          uint8 x=Right[Row];
          //          for(i=0;i<n;i++)
          //          {
          //            if(Img[Row][x-i]==ImgWhite)   //统计右方白点数目，加入全白限制
          //              WhitePoint++;             //唔，不知道如果再换为100度镜头后，右方是否为全白
          //          } 
          //防误判
          if(Lost_L[Row-1]==1 && Lost_L[Row-2]==1)
          {                                                //和正常弯道区别开
            flag_guai=1;
            break;
          }
        }
        
      }
    }  
    if(flag_guai==1 )//找到拐点
    {
      flag_guai=0;
      int j;
      int temp_x,temp_y;
      RoadType=CrossL;
      float xielv3=10/(Right[Row+10]-Right[Row]);
      for(j=Row+9;j>Sight_Init;j--)
      {
        int right=(j-Row-10)*Cross_Xielv2+Right[Row+10];       //按斜率补线
        if(right<Left_Init)
          right=Left_Init;
        Right[j] = right;
        Middle[j]=fanjiaozheng_x(j,Right[j],-ROAD_HALF);    //反校正补线
        Left[j]=fanjiaozheng_x(j,Right[j],-ROAD_W);
        if(Middle[j]<Left_Init)
          Middle[j]=Left_Init;
        if(Left[j]<Left_Init)
          Left[j]=Left_Init;
        Lost_L[j]=Lost_R[j]=0;
      }
      Sight_L=Sight_R=Sight_Init;
      return 1;
    }
  }
  return 0;
}
/****************************
作用：计算中线偏差
返回：图像偏差
****************************/
float jisuan_piancha()
{
  int y=0;
  int Sight_Temp;
  float piancha_avr=0;
  float piancha=0;
  ValidLine=0;         //有效行清零
  if(Sight>Sight_Init_Turn)
    Sight_Temp = Sight;
  else
    Sight_Temp = Sight_Init_Turn;
  for(y=119;y>=Sight_Temp;y--)  //搜索边界最远用到1米5，但计算偏差最远用到1米2
  {
    //  if(SelectRowFlag[y]==1)      //只计算特定行的偏差
    {
      // if((Lost_L[y]==LostWhite && Lost_R[y]==LostWhite) || (Lost_L[y]==LostBlack && Lost_R[y]==LostBlack)||(Middle[y]==Left_Init) || (Middle[y]==Right_Init))
      if((Lost_L[y]==LostWhite && Lost_R[y]==LostWhite) || (Lost_L[y]==LostBlack && Lost_R[y]==LostBlack))
        
      {//本行不参与偏差计算
      }      
      else
      {
        //piancha+=((int)(Middle[y]-80)*Img_P[y/10]);
        piancha+=((int)(Middle[y]-69)*Img_P[y/10]);
        ValidLine++;              //累计有效行数
      }
    }
  }
  piancha_avr=(piancha)/ValidLine;    //平均偏差，后期要改各行权重比
  
  if(piancha_avr>54)
    piancha_avr=54;
  if(piancha_avr<-54)
    piancha_avr=-54;
  
//  if(piancha_avr>65)
//    piancha_avr=65;
//  if(piancha_avr<-65)
//    piancha_avr=-65;
  return piancha_avr;//+10; //解决摄像头歪
}
/****************************
作用：起跑线检测
****************************/
void StartLineCheck()
{
  if(CarType == Front && CarState == CarStart)
  {
    if(Distance>200)
    {
      RoadScan.PlanStart();
      if(RoadScan.sRes.state == Exist)
      {
        FlagStart = 2;
        CarState = CarStop;
        ImgStopNum = ImgNum;
        led(LED0,LED_ON); 
      }
      else
      {
        led(LED0,LED_OFF);
      }        
    }
    
  }
}


/****************************
未验证
作用：直线入弯判断
****************************/
void GoInBendCheck()
{
  uint8 YuZhi = 1;     
  uint8 Error_YuZhi = 40;
  uint8 Sight_YuZhi = 34;
  if(StraightNum>YuZhi)    //之前连续几幅图都是直道
  {
    if(Sight>=Sight_YuZhi)
    {
      CurveType = GoInBend; 
      if(Distance_GoInBend == 0)
        Distance_GoInBend = Distance;
    }
  }
  if(Distance_GoInBend!=0 && Distance - Distance_GoInBend>60)
  {
    CurveType = Curve_Null;
    StraightNum = 0;
  }
  if(MiddleError<0 && abs_sub(Lost_L_Num,116-Sight_L)<4)
  {
    StraightNum = 0;
    CurveType = Bend;
  }
  if(MiddleError>0 && abs_sub(Lost_R_Num,116-Sight_R)<4)
  {
    CurveType = Bend;
    StraightNum = 0;
  }
  
  if(RoadType == GoOutBend || RoadType == CrossL || RoadType == CrossR)
    StraightNum = 0;
  if(AbsMiddleError>38)
    StraightNum = 0;
}
/****************************
未验证
作用：出弯判断
****************************/
void GoOutBendCheck()
{
  if(BendNum>3)
  {
    if(Sight<40 && (ImgNum-Img_GoOutBend)>10)//后一个判断是为了保证出弯加速只作用一次，不过只一次真的好吗。。。
    {
      Flag_GoOutBend = 1;
      Img_GoOutBend = ImgNum;
      RoadType = GoOutBend;
    }
  }
}
/****************************
未验证
作用：弯道中判断
****************************/
void BendCheck()
{
  //  uint8 Yuzhi=10;
  //  if(AbsMiddleError>=Yuzhi)
  //  {
  //    BendNum++;
  //    StraightNum=0;
  //    RoadType = Bend;
  //  }
  //  else
  //    BendNum=0;
}
/****************************
未验证
作用：直道判断
****************************/
void StraightCheck()
{
  {   
    uint8 Yuzhi=10;
    if(AbsMiddleError<Yuzhi && Sight - Sight_Init<2)//偏差小，前瞻远
    {
      //      float XieLv_YuZhi = 0.2;
      //      float XieLv_Top = (float)((int)Middle[Sight_Init_Turn]-(int)Middle[(Sight_Init_Turn+119)/2])/(float)((Sight_Init_Turn+119)/2-Sight_Init_Turn);
      //      float XieLv_Bottom = (float)((int)Middle[(Sight_Init_Turn+119)/2]-(int)Middle[119])/(float)(119-(Sight_Init_Turn+119)/2);
      //      if(XieLv_Top>=0 && XieLv_Bottom>=0 && Myabsfloat(XieLv_Top-XieLv_Bottom)<XieLv_YuZhi)
      {
        CurveType = Straight;
        if(Count_Total-Count_Total_Last>335)//每5CM更新一次
        {
          Count_Total_Last = Count_Total;
          StraightNum++;
          BendNum=0; 
        }
      } 
      //      else if(XieLv_Top<=0 && XieLv_Bottom<=0 && Myabsfloat(XieLv_Top-XieLv_Bottom)<XieLv_YuZhi)
      //      {
      //        StraightNum++;
      //        BendNum=0;    
      //      } 
    }
    else
    {
      CurveType = Curve_Null;
      StraightNum = 0;
    }
      if(StraightNum>0)                //至少连续两次判断为直道，才将赛道类型赋值为直道
    {                                //毕竟判断为直道后，电机速度会瞬加，这样做更保险
     //  CurveType = Straight;
    }
  }
}
//入十字检测，十字特殊处理，采用定值P方案
void CrossCheck()
{
  uint8 Num=0;
  uint8 flag = 0;
  if(Distance_Cross == 0 || (Distance_Cross!=0 && Distance - Distance_Cross>60))
  {
    for(uint8 y = 116;y>80;y--)
    {
      if(Lost_L[y] == LostWhite && Lost_R[y] == LostWhite)
        Num++;
    }
    if(Num >29)//底部两边都丢线，且很多行都是这样
    {
      // led(LED0,LED_ON);
      // if(abs_sub(Middle[Sight],Middle[119])>15)//十字后是弯道而非直道
      {
        //   flag = 1;
        RoadType = Cross;
        Distance_Cross = Distance;//记录路程主要是为了将出十字和入十字区别开
        CrossNum++;
        if(CrossNum == 2)
        {
          Distance_Cross_GoOut = Distance;
          CrossNum = 0;
          RoadType = Normal;
        }
      }
    }
  }
  if(Distance_Cross_GoOut != 0)
  {
    if(Lost_L[116] ==0 && Lost_R[116] == 0)
    {
      Distance_Cross = 0;
      Distance_Cross_GoOut = 0;
      CrossNum = 0;
      RoadType = Normal;     
    }
  }
  
  
}
//赛道类型滚动更新
void CurveType_Roll()
{
  //  int YuZhi = 10;               //每走过一定的路程更新一次。
  //  if(Distance - Distance_Last>YuZhi)
  //  {
  //    Distance_Last = Distance;
  //    for(uint8 i = 1;i<CurveTypeRecordMax;i++)
  //    {
  //      CurveType_History[i] = CurveType_History[i-1];
  //    }
  //    CurveType_History[0] = CurveType;  
  //  }
  
}
//加宽超车区识别
uint8 OvertakeAreaFind(uint8 judge)
{
  if(CarType == Front && judge == 0 && RoadType == 0)
  {
    Road_OvetakeFind_X    = 0;
    int YuZhi_Road = 12;
    float YuZhi_XieLv = 0.2;
    int YuZhi_WhitePoint = 52;
    Y_Overtake_Start = Sight;
    // 看的比较远             
    if(Sight == Sight_Init) 
    {
      int YuZhi_Middle = 10;
      uint8 Num = 0;
      int WhitePoint = 0;
      for(uint8 y = Sight;y<Sight+20;y++)
      {  
        Road_OvetakeFind_X = real_x[y][Right[y]]-real_x[y][Middle[y]];
        if(abs_sub(Road_OvetakeFind_X,ROAD_W)<YuZhi_Road)
        {
          Num++;
          if(Num == 1)
            Y_Overtake_Start = y;
          for(uint8 x = Right[y];x>Left[y];x--)
          {
            if(Img[y][x] == ImgWhite)
              WhitePoint++;
          }
        }
      }
      if(Num >12 && WhitePoint>Num*YuZhi_WhitePoint)
      {
        XieLv_Overtake_Middle = (Middle[Y_Overtake_Start+3]-Middle[Y_Overtake_Start+8])/5.0;
        XieLv_Overtake_Left   = (Left[90] - Left[105])/15.0;
        XieLv_Overtake_Right   = (Right[90] - Right[105])/15.0;
        if(XieLv_Overtake_Middle>0 && Myabsfloat(XieLv_Overtake_Middle-XieLv_Overtake_Left)<YuZhi_XieLv)
        {
          RoadType = Overtake_Left;
          
          return 1;
        }
        if(XieLv_Overtake_Middle<0 && Myabsfloat(XieLv_Overtake_Middle-XieLv_Overtake_Right)<YuZhi_XieLv)
        {
          RoadType = Overtake_Right;
          return 1;
        }
      }
    }  
  }
  return 0;
}
//进入超车区
uint8 OvertakeArea_ing(uint8 judge)
{
  if(judge == 0 && RoadType == Overtake_Left)
  {
    int Num_White = 0;
    for(uint8 x = 80;x>0;x--)
    {
      if(Img[119][x] == ImgWhite)
        Num_White++;
    }
    if(Num_White>70)
    {
      int Num   = 0;
      int YuZhi = 15;
      uint8 flag = 0;
      for(uint8 y = 119;y>89;y--)
      {
        if(Lost_L[y] == LostWhite)
          Num++;
      }
      for(uint8 y =116;y>Sight_L+3;y--)//没有突然向右的点，情景：进入超车区前车体较考左，右边底部很多找不到，可能会误判为进入超车区
      {
        if(Left[y]<Right[y+1] && Left[y+1]-Left[y]>20 && Lost_L[y+1] == 0)
        {
          flag = 1;
          break;
        }
      }
      if(Num>YuZhi && flag ==0)
      {
        RoadType = Overtake_Left_ing;
        Sight = Sight_L;
        
        return 1;      
      }
    }
  }
  if(judge == 0 && RoadType == Overtake_Right)
  {
    int Num_White = 0;
    for(uint8 x = 159;x>80;x--)
    {
      if(Img[119][x] == ImgWhite)
        Num_White++;
    }
    if(Num_White>70)
    {
      int Num   = 0;
      int YuZhi = 15;
      uint8 flag = 0;
      for(uint8 y = 119;y>89;y--)
      {
        if(Lost_R[y] == LostWhite)
          Num++;
      }
      for(uint8 y =116;y>Sight_R+3;y--)//没有突然向右的点，情景：进入超车区前车体较考左，右边底部很多找不到，可能会误判为进入超车区
      {
        if(Right[y]>Right[y+1] && Right[y]-Right[y+1]>20 && Lost_R[y+1] == 0)
        {
          flag = 1;
          break;
        }
      }
      if(Num>YuZhi && flag ==0)
      {
        RoadType = Overtake_Right_ing;
        Sight = Sight_R;
        
        return 1;      
      }
    }
  }
  return 0;
}
//从超车区返回
//uint8 OvertakeArea_Return(uint8 judge)
//{
//  if(judge == 0 && CarType == Front && RoadType == Overtake_Left_ing)
//  {
//    if(Sight_L>45)              //这个系数决定了从哪里开始返回
//      RoadType = Overtake_Left_Return;
//  }
//  if(judge == 0 && RoadType == Overtake_Right_ing)
//  {
//    if(Sight_L>45)              //这个系数决定了从哪里开始返回
//      RoadType = Overtake_Right_Return;
//  }
//  return 0;
//}
//结束超车
void OvertakeArea_Over()
{
  if(RoadType == Overtake_Right_Rerturn_Execute)
  {
    if(Lost_R[116] == 0 && Lost_R[115] == 0)
      RoadType = Normal;
  }
}
//超车区后车变前车
void Overtake_LaterToFront()
{//右超车区
  if(CarType == Later && RoadType_AnotherCar == Overtake_Right_ing)
  {
    int YuZhi = 110;
    memset(Lost_R,LostWhite,CAMERA_H);
    for(uint8 y = 119;y>75;y-=2)
    {
      for(uint8 x = 80;x<Right_Init;x++)
      {
        if(Img[y][x]==ImgBlack)//找到黑点
        {//进一步确认
          if(Img[y][x+2]==ImgBlack && Img[y][x+1]==ImgBlack
             && Img[y][x-2]==ImgWhite && Img[y][x-1]==ImgWhite)
          {
            Lost_R[y] = 0;
            Right[y] = x-2;
            break;
          }
        }
      }
      if(Lost_R[y] == 0 && Right[y]<150)
        return ;
    }
    if(Distance_Overtake_Right_GoStraight == 0)
      Distance_Overtake_Right_GoStraight = Distance;//记录路程，再过一段距离再变为走中间模式
  }
  
  //左超车区
  if(CarType == Later && RoadType_AnotherCar == Overtake_Left_ing)
  {
    int YuZhi = 110;
    memset(Lost_L,LostWhite,CAMERA_H);
    for(uint8 y = 119;y>75;y-=2)
    {
      for(uint8 x = 80;x>Left_Init;x--)
      {
        if(Img[y][x]==ImgBlack)//找到黑点
        {//进一步确认
          if(Img[y][x-2]==ImgBlack && Img[y][x-1]==ImgBlack
             && Img[y][x+2]==ImgWhite && Img[y][x+1]==ImgWhite)
          {
            Lost_L[y] = 0;
            Left[y] = x+2;
            break;
          }
        }
      }
      if(Lost_L[y] == 0 && Left[y]>10)
        return ;
    }
    if(Distance_Overtake_Left_GoStraight == 0)
      Distance_Overtake_Left_GoStraight = Distance;//记录路程，再过一段距离再变为走中间模式
    return ;
  }
}
//前车进入正常赛道
void Overtake_FrontReturn()
{
  //
  if(CarType == Front && RoadType == Overtake_Right_GoStraight)
  {
    //右边底部能看到
    if(Lost_R[CAMERA_H-4] == 0)
      RoadType = Normal;
  }
  if(CarType == Front && RoadType == Overtake_Left_GoStraight)
  {
    //右边底部能看到
    if(Lost_L[CAMERA_H-4] == 0)
      RoadType = Normal;
  }
}
void Overtake_LaterReturn()
{
  //右超车区
  if(RoadType == Overtake_Right_ing)
  {
    if(Lost_L[CAMERA_H-4] == 0 && Lost_L[CAMERA_H-5] == 0 && Lost_L[CAMERA_H-6] == 0)
    {  
      RoadType = Overtake_Right_Rerturn_Prepare;
    }
  }
  if(RoadType == Overtake_Right_Rerturn_Prepare)
  {
    if(Lost_L[CAMERA_H-4] == LostWhite && Lost_L[CAMERA_H-5] == LostWhite && Lost_L[CAMERA_H-6] == LostWhite)
    { 
      RoadType = Overtake_Right_Rerturn_Execute;  
      CarType = Later;
    }
  }
  
  //左超车区
  if(RoadType == Overtake_Left_ing)
  {
    if(Lost_R[CAMERA_H-4] == 0 && Lost_R[CAMERA_H-5] == 0 && Lost_R[CAMERA_H-6] == 0)
    {  
      RoadType = Overtake_Left_Rerturn_Prepare;
    }
  }
  if(RoadType == Overtake_Left_Rerturn_Prepare)
  {
    if(Lost_L[CAMERA_H-4] == LostWhite && Lost_L[CAMERA_H-5] == LostWhite && Lost_L[CAMERA_H-6] == LostWhite)
    { 
      RoadType = Overtake_Left_Rerturn_Execute;  
      CarType = Later;
    }
  }
}
void Overtake_Middle()
{
  if(CarType == Front)
  {
    //if(Distance_Overtake != 0)
    if(RoadType == Overtake_Left)
    {
      int Road_Correct = -ROAD_HALF - 7;
      {
        for(uint8 y = 119;y>=Sight_R;y--)
        {
          Middle[y] = fanjiaozheng_x(y,Right[y],Road_Correct);
        }
      }
    }
    if(RoadType == Overtake_Right)
    {
      int Road_Correct = ROAD_HALF + 7;
      {
        for(uint8 y = 119;y>=Sight_L;y--)
        {
          Middle[y] = fanjiaozheng_x(y,Left[y],Road_Correct);
          
        }
      }
    }
    if(RoadType == Overtake_Left_ing)
    {
      int Road_Correct = ROAD_HALF+25;
      for(uint8 y = 119;y>=Sight_R;y--)
      {
        if(abs_sub(real_x[y][Right[y]],real_x[y][Left[y]])>60)
          Middle[y] = fanjiaozheng_x(y,Left[y],Road_Correct);
        if(Lost_L[y] == LostWhite || (Lost_R[y] == 0 && real_x[y][Right[y]]-real_x[y][Left[y]]<60) || Lost_L[y] == LostBlack)
          Lost_R[y] = Lost_L[y] = LostWhite;
      }
    }
    if(RoadType == Overtake_Right_ing)
    {
      int Road_Correct = -ROAD_HALF - 25;
      for(uint8 y = 119;y>=Sight_L;y--)
      {
        if(abs_sub(real_x[y][Right[y]],real_x[y][Left[y]])>60)
          Middle[y] = fanjiaozheng_x(y,Right[y],Road_Correct);
        if(Lost_R[y] == LostWhite || (Lost_L[y] == 0 && real_x[y][Right[y]]-real_x[y][Left[y]]<60) || Lost_R[y] == LostBlack)
          Lost_R[y] = Lost_L[y] = LostWhite;
      }
    }
    
    //    if(RoadType == Overtake_Left_Return)
    //    {
    //      for(uint8 y = 119;y>Sight_L;y--)
    //        Lost_L[y] = Lost_R[y] = LostWhite;        //为了计算偏差时不使用这些行
    //      for(uint8 y = Sight_L;y>Sight_Init;y--)
    //      {
    //        for(uint8 x = 157;x>60;x--)
    //        {
    //          if(Img[y][x]==ImgBlack)//找到黑点
    //          {//进一步确认
    //            if(Img[y][x-1]==ImgBlack && Img[y][x-2]==ImgBlack && Img[y][x+1]==ImgWhite && Img[y][x+2]==ImgWhite)
    //            { 
    //              Left[y]=x+2;
    //              Lost_L[y]=0;        //丢线标志位清零
    //              Middle[y] = fanjiaozheng_x(y,Left[y],25); 
    //              break;        //退出循环 
    //            }
    //          }
    //        }
    //        
    //      }
    //    }
    //    if(RoadType == Overtake_Right_Return)
    //    {
    //      for(uint8 y = 119;y>Sight_R;y--)
    //        Lost_L[y] = Lost_R[y] = LostWhite;        //为了计算偏差时不使用这些行
    //      for(uint8 y = Sight_R;y>Sight_Init;y--)
    //      {
    //        for(uint8 x = 2;x<100;x++)
    //        {
    //          if(Img[y][x]==ImgBlack)//找到黑点
    //          {//进一步确认
    //            if(Img[y][x+1]==ImgBlack && Img[y][x+2]==ImgBlack && Img[y][x-1]==ImgWhite && Img[y][x-2]==ImgWhite)
    //            { 
    //              Right[y]=x-2;
    //              Lost_R[y]=0;        //丢线标志位清零
    //              Middle[y] = fanjiaozheng_x(y,Right[y],-25); 
    //              break;        //退出循环 
    //            }
    //          }
    //        }
    //      }
    //    }
    
  }
  if(CarType == Later)
  {
    if(RoadType_AnotherCar == Overtake_Left || RoadType_AnotherCar == Overtake_Left_ing)
    {
      int Road_Correct = -11;
      for(uint8 y = 119;y>Sight_R;y--)
      {
        Middle[y] = fanjiaozheng_x(y,Right[y],Road_Correct);
      }
    }
    if(RoadType_AnotherCar == Overtake_Right || RoadType_AnotherCar == Overtake_Right_ing)
    {
      int Road_Correct = 11;
      for(uint8 y = 119;y>Sight_L;y--)
      {
        Middle[y] = fanjiaozheng_x(y,Left[y],Road_Correct);
      }
    }
  }
  float Distance_YuZhi = 10;
  if(Distance_Overtake_Right_GoStraight!=0 && Distance-Distance_Overtake_Right_GoStraight>Distance_YuZhi)
  {
    CarType  = Front; 
    Distance_Overtake_Right_GoStraight = 0;
    RoadType = Overtake_Right_GoStraight;  
  }
  if(RoadType == Overtake_Right_GoStraight)
  {
    for(uint8 y = 119;y>Sight_L;y--)
    {
      Middle[y] = fanjiaozheng_x(y,Left[y],ROAD_HALF);
      Right[y]  = fanjiaozheng_x(y,Left[y],ROAD_W);
    }
  }
  if(Distance_Overtake_Left_GoStraight!=0 && Distance-Distance_Overtake_Left_GoStraight>Distance_YuZhi)
  {
    CarType  = Front; 
    Distance_Overtake_Left_GoStraight = 0;
    RoadType = Overtake_Left_GoStraight;  
  }
  if(RoadType == Overtake_Left_GoStraight)
  {
    for(uint8 y = 119;y>Sight_R;y--)
    {
      Middle[y] = fanjiaozheng_x(y,Right[y],-ROAD_HALF);
      Left[y]  = fanjiaozheng_x(y,Right[y],-ROAD_W);
    }
  }
}

//void Overtake_Cross_BoundryFind()
//{
//  if(RoadScan.tRes.state == Exist)
//  {
//    for(uint8 y = 119;y>)
//  }
//}
//重新找一遍右边界，主要因为前车和赛道边缘混在一起。
void GenZong_Again_Right()
{
  int Window = 10;
  uint8 Final_R=Right_Init,Start_R;
  uint8 x = 0;
  memset(Right,Right_Init,CAMERA_H-3);
  memset(Lost_R,LostWhite,CAMERA_H-3);
  Last_R=(Right[119]+Right[118]+Right[117])/3;
  uint8 Y_Stop;
  uint8 YuZhi = 44;
  uint8 YuZhi_Sight = 44;
  uint8 Flag_NotRefind = 0;
  if(Light_Y-Sight<YuZhi && Sight>YuZhi_Sight)
  {
    Y_Stop = Sight;//已经进入弯道，顶部不应该被找到，不应向右发展
    Flag_NotRefind = 1;
  }
  else if(Light_X>142)
  {
    Y_Stop = Sight;
    Flag_NotRefind = 1;
  }
  else
    Y_Stop = Light_Y;
  for(Row = 116;Row>Sight_Init;Row--)
  {
    if(Last_R+Window<Right_Init)
      Final_R=Last_R+Window;
    else
      Final_R=Right_Init;
    if(Last_R-Window>Left_Init)
      Start_R=Last_R-Window;
    else
      Start_R=Left_Init;
    //右边界从左向右搜 
    for(x=Start_R;x<Final_R;x++) 
    {
      if(Img[Row][x]==ImgBlack)//找到黑点
      {//进一步确认
        if(Img[Row][x+1]==ImgBlack && Img[Row][x+2]==ImgBlack && Img[Row][x-1]==ImgWhite && Img[Row][x-2]==ImgWhite && Img[Row][x-3]==ImgWhite)
        { 
          if(Row<=Light_Bottom && Row>=Light_Top)
          {
            if(x-2<Light_Right)
            {}
            else
            {
              Right[Row]=x-2;
              Lost_R[Row]=0;        //丢线标志位清零
              break;        //退出循环 
            }                  
          }
          else
          {
            Right[Row]=x-2;
            Lost_R[Row]=0;
          }
          if(Row>Y_Stop && Right[Row]<Right[Row+1])
          {
            Right[Row] = Right[Row+1];
          }
          break;//退出循环
        }
      }           
    }
    Last_R=Right[Row];     //保存上一次的值
    if(Flag_NotRefind == 0 && Right[Row]==Right_Init)//说明右边界没有提取到
    {
      Last_R=Right[Row]=Refind_R(Row);    
    }
    Right_Correct(Row);
  }
}
//重新找一遍左边界，主要因为前车和赛道边缘混在一起。
void GenZong_Again_Left()
{
  int Window = 10;
  int Final_L=Left_Init,Start_L;
  int x = 0;
  memset(Left,Left_Init,CAMERA_H-3);
  memset(Lost_L,LostWhite,CAMERA_H-3);
  Last_L=(Left[119]+Left[118]+Left[117])/3;
  int Y_Stop;
  int YuZhi = 44;
  int YuZhi_Sight = 44;
  uint8 Flag_NotRefind = 0;
  if(Light_Y-Sight<YuZhi && Sight>YuZhi_Sight)
  { 
    Y_Stop_GenZongAgain = Sight;//已经进入弯道，顶部不应该被找到，不应向右发展
    Flag_NotRefind = 1;
  }
  else if(Light_X<17)
  {
    Y_Stop_GenZongAgain = Sight;
    Flag_NotRefind = 1;
  }
  else
    Y_Stop_GenZongAgain = Light_Y;
  for(Row = 116;Row>Sight_Init;Row--)
  {
    //左边界从右向 左搜 
    if(Last_L-Window>Left_Init)
      Final_L=Last_L-Window;       
    if(Last_L+Window<Right_Init)
      Start_L=Last_L+Window;
    else
      Start_L=Right_Init;
    for(x=Start_L;x>Final_L;x--)     
    {
      if(Img[Row][x]==ImgBlack)//找到黑点
      {//进一步确认
        if(Img[Row][x-1]==ImgBlack && Img[Row][x-2]==ImgBlack && Img[Row][x+1]==ImgWhite && Img[Row][x+2]==ImgWhite  && Img[Row][x+3]==ImgWhite)
        {
          if(Row<=Light_Bottom && Row>=Light_Top)
          {
            if(x+2>Light_Left)
            {}
            else
            {
              Left[Row]=x+2;
              Lost_L[Row]=0;        //丢线标志位清零
              break;        //退出循环 
            }                  
          }
          else
          {
            Left[Row]=x+2;
            Lost_L[Row]=0;        //丢线标志位清零
          }
          if(Row>Y_Stop_GenZongAgain && Left[Row]>Left[Row+1])
          {
            Left[Row] = Left[Row+1];
            Lost_L[Row] = LostWhite;
          }
          break;        //退出循环 
        }
      }           
    }
    Last_L=Left[Row];     //保存上一次的值
    if(Flag_NotRefind == 0 && Left[Row]==Left_Init)//说明没有提取到边界
    {
      Last_L=Left[Row]=Refind_L(Row);     //再次寻找
    }
    Left_Correct(Row);
  }
}
//后车纠正错误的左边界
void Left_Correct(uint8 y)
{
  
  int Change_YuZhi = 13;
  // if()
  //白点上方
  if((y<Light_Bottom) && (Left[y]>(Light_Left-10)))
  {
    //  if(abs_sub(Left[y],Left[y+1])>Change_YuZhi)
    if((abs_sub(Right[y],Light_X)>abs_sub(Light_X,Left[y])) && (abs_sub(Left[y+1],Left[y])>Change_YuZhi) && Lost_L[y] == 0)
    {
      //      if(y<80 && Lost_R[y+25] == LostWhite && Lost_R[y+28] == LostWhite && Lost_L[y+5] == LostWhite  && Lost_L[y+10] == LostWhite  && Lost_L[y+15] == LostWhite  && Lost_L[y+20] == LostWhite)
      //      {}//斜出十字
      //      else
      {
        int Start = Left[y]-4;
//        if(Left[y+1]<10)
//          Last_L = Left[y] = Left[y+1];//4.19.12.1211
//        else
          Last_L = Left[y] = Left_Init;//先恢复默认值，等找到后自会更新
        if(Img[y][Left_Init] == ImgWhite)
          Lost_L[y] = LostWhite;
        if(Img[y][Left_Init] == ImgBlack)
          Lost_L[y] = LostBlack;        for(int x = Start;x>Left_Init;x--)
          {
            if(Img[y][x]==ImgBlack)//找到黑点
            {//进一步确认
              if(Img[y][x-3]==ImgBlack && Img[y][x-2]==ImgBlack && Img[y][x-1]==ImgBlack
                 && Img[y][x+3]==ImgWhite && Img[y][x+2]==ImgWhite && Img[y][x+1]==ImgWhite)
              { 
                if(abs_sub(x,Left[y+1])<=Change_YuZhi)
                { 
                  Last_L = Left[y]=x+2;
                  Lost_L[y]=0;        //丢线标志位清零
                  break;        //退出循环 
                }
              }
            }  
          } 
      }
      ErrorType = LeftCorrect_1; 
    }
  }
  //白点上方2
  if((y<Light_Bottom) && (Left[y]>(Light_Left-10)))
  {
    int YuZhi = 5;
    if((abs_sub(Right[y],Light_X)>abs_sub(Light_X,Left[y])) && (abs_sub(Left[y+1],Left[y])>YuZhi) && Lost_L[y] == 0)
    {
      uint8 Num = 0;
      for(uint8 y1 = y+1;y1<y+12;y1++)
      {
        if(Left[y1] == Left_Init && Img[y1][Left_Init] == ImgBlack)
          Num++;
      }
      if(Num>7)
      {
        int Start = Left[y]-4;
        Last_L = Left[y] = Left_Init;//先恢复默认值，等找到后自会更新
        if(Img[y][Left_Init] == ImgWhite)
          Lost_L[y] = LostWhite;
        if(Img[y][Left_Init] == ImgBlack)
          Lost_L[y] = LostBlack;
        for(int x = Start;x>Left_Init;x--)
        {
          if(Img[y][x]==ImgBlack)//找到黑点
          {//进一步确认
            if(Img[y][x-3]==ImgBlack && Img[y][x-2]==ImgBlack && Img[y][x-1]==ImgBlack
               && Img[y][x+3]==ImgWhite && Img[y][x+2]==ImgWhite && Img[y][x+1]==ImgWhite)
            { 
              if(abs_sub(x,Left[y+1])<=Change_YuZhi)
              { 
                Last_L = Left[y]=x+2;
                Lost_L[y]=0;        //丢线标志位清零
                break;        //退出循环 
              }
            }
          }  
        }  
        ErrorType = LeftCorrect_2; 
      }
    }
  }
  //白块上的点被当做边界
  if((y>Light_Top-2) && y<Light_Bottom+2 && (abs_sub(Right[y],Light_X)>abs_sub(Light_X,Left[y])) && (abs_sub(Left[y],Left[y+1])<=Change_YuZhi) && Lost_L[y] == 0 && Left[y]>=Light_Left-3)
  {
    int Start = Left[y]-4;
    Last_L = Left[y] = Left_Init;//先恢复默认值，等找到后自会更新
    if(Img[y][Left_Init] == ImgWhite)
      Lost_L[y] = LostWhite;
    if(Img[y][Left_Init] == ImgBlack)
      Lost_L[y] = LostBlack;
    for(int x = Start;x>Left_Init;x--)
    {
      if(Img[y][x]==ImgBlack)//找到黑点
      {//进一步确认
        if(Img[y][x-3]==ImgBlack && Img[y][x-2]==ImgBlack && Img[y][x-1]==ImgBlack
           && Img[y][x+3]==ImgWhite && Img[y][x+2]==ImgWhite && Img[y][x+1]==ImgWhite)
        { 
          if(x<Light_Left-4)
          {
            Last_L = Left[y]=x+2;
            Lost_L[y]=0;        //丢线标志位清零
            break;        //退出循环 
          }
        }
      }  
    }
    ErrorType = LeftCorrect_3;
  }
  //前轮左轮内侧误判为边界
  if(y<Light_Bottom+8 && y>Light_Top && Left[y]>Left_Init && Left[y]<Light_X)
  {
    uint8 flag = 0;
    uint8 flag2 = 0;
    for(uint8 x = Left[y];x<Left[y]+10;x++)
    {
      if(Img[y][x] == ImgBlack)
      {
        flag = 1;
        break;
      }
    }
    for(uint8 y1 = y;y1<y+5 && y1<119;y1++)
    {
      if(Img[y1][Left[y]] == ImgBlack)
      {
        flag2 = 1;
        break;
      }
    }  
    if(flag == 1)
    {
      uint8 Num = 0;
      for(uint8 i = y;i<119;i++)
      {
        if(Lost_L[i] == LostWhite && Img[i][Left[i]] == ImgWhite)
          Num++;
      }
      if(Num>2 && abs_sub(Left[y],Left[y+2])>15)
      {//y+2 -- 4.28.2.710 
        Last_L = Left[y] = Left_Init;
        if(Img[y][Left_Init] == ImgWhite)
          Lost_L[y] = LostWhite;
        if(Img[y][Left_Init] == ImgBlack)
          Lost_L[y] = LostBlack;
        ErrorType = LeftCorrect_4;      
      }
      else if(Num>8)
      {
        Last_L = Left[y] = Left_Init;
        if(Img[y][Left_Init] == ImgWhite)
          Lost_L[y] = LostWhite;
        if(Img[y][Left_Init] == ImgBlack)
          Lost_L[y] = LostBlack;
        ErrorType = LeftCorrect_4;
      }}
  }
  //U型弯末端，找不到光源，只看到前车后轮
  if(RoadScan.Res.state != Exist || Light_X<20)
  {
    uint8 YuZhi_X = 40;
    uint8 YuZhi_Y = 60;
    uint8 YuZhi_Sight = 49;
    if(Light_X <YuZhi_X && Light_Y>YuZhi_Y && Sight>YuZhi_Sight)
    {
      if(Left[y]>Left[y+1])
      {
        Last_L = Left[y] = Left[y+1];
        Lost_L[y+1] = Lost_L[y] = LostWhite;
        //y+1行也变成LostWhite，是有时候最初的一行能找到，但找的是车
        ErrorType = LeftCorrect_5;
      }
    }          
  }
  //弯道末端，白点上方找错
  if(Light_X<34)
  {
    if(Left[y]>Left[y+1])
    {
      Last_L = Left[y] = Left[y+1];
      Lost_L[y+1] = Lost_L[y] = LostWhite;
      //y+1行也变成LostWhite，是有时候最初的一行能找到，但找的是车
      ErrorType = LeftCorrect_6;
    }
  }
}
//后车纠正错误的右边界
void Right_Correct(uint8 y)
{
  int Change_YuZhi = 14;
  //白点上方
  //  if(0)
  if(y<Light_Bottom && (Right[y]<(Light_Right+10)) )
  {  
    if((abs_sub(Right[y],Light_X)<abs_sub(Light_X,Left[y])) && (abs_sub(Right[y],Right[y+1])>Change_YuZhi) && Lost_R[y] == 0)
    {
      //      if(y<80 && Lost_L[y+25] == LostWhite && Lost_L[y+28] == LostWhite && Lost_R[y+5] == LostWhite  && Lost_R[y+10] == LostWhite  && Lost_R[y+15] == LostWhite  && Lost_R[y+20] == LostWhite)
      //      {}//斜出十字
      //      else
      {
        int Start = Right[y]+4;
//        if(Right[y+1]>150)
//          Last_R = Right[y] = 150;//4.19.12.1211
//        else
          Last_R = Right[y] = Right_Init;//先恢复默认值，等找到后自会更新
        if(Img[y][Right_Init] == ImgWhite)
          Lost_R[y] = LostWhite;
        if(Img[y][Right_Init] == ImgBlack)
          Lost_R[y] = LostBlack;
        for(int x = Start;x<Right_Init;x++)
        {
          if(Img[y][x]==ImgBlack)//找到黑点
          {//进一步确认
            if(Img[y][x+3]==ImgBlack && Img[y][x+2]==ImgBlack && Img[y][x+1]==ImgBlack
               && Img[y][x-4]==ImgWhite  && Img[y][x-3]==ImgWhite && Img[y][x-2]==ImgWhite && Img[y][x-1]==ImgWhite)
            { 
              //  Road_X = real_x[y][x] - real_x[y][Left[y+1]];
              if(abs_sub(x,Right[y+1])<Change_YuZhi)
              { 
                Last_R = Right[y]=x-2;
                Lost_R[y]=0;        //丢线标志位清零
                break;        //退出循环 
              }
            }
          }
        }  
      }  
      ErrorType = RightCorrect_1;
    }
  }
  //白点上方2
  // if(0)
  if(y<Light_Bottom && (Right[y]<(Light_Right+10)) )
  {
    if((abs_sub(Right[y],Light_X)<abs_sub(Light_X,Left[y])) && Lost_R[y] == 0)
    {
      uint8 Num = 0;
      for(uint8 y1 = y+1;y1<y+12;y1++)
      {
        if(Right[y1] == Right_Init && Img[y1][Right_Init] == ImgBlack)
          Num++;
      }
      if(Num>7)
      {
        int Start = Right[y]+4;
        Last_R = Right[y] = Right_Init;//先恢复默认值，等找到后自会更新
        if(Img[y][Right_Init] == ImgWhite)
          Lost_R[y] = LostWhite;
        if(Img[y][Right_Init] == ImgBlack)
          Lost_R[y] = LostBlack;
        for(int x = Start;x<Right_Init;x++)
        {
          if(Img[y][x]==ImgBlack)//找到黑点
          {//进一步确认
            if(Img[y][x+3]==ImgBlack && Img[y][x+2]==ImgBlack && Img[y][x+1]==ImgBlack
               && Img[y][x-4]==ImgWhite  && Img[y][x-3]==ImgWhite && Img[y][x-2]==ImgWhite && Img[y][x-1]==ImgWhite)
            { 
              //  Road_X = real_x[y][x] - real_x[y][Left[y+1]];
              if(abs_sub(x,Right[y+1])<Change_YuZhi)
              { 
                Last_R = Right[y]=x-2;
                Lost_R[y]=0;        //丢线标志位清零
                break;        //退出循环 
              }
            }
          }  
        }  
        ErrorType = RightCorrect_2;
      }
    }
  }
  //白块上的点误判为边界
  // if(0)
  if((y>(Light_Top-2)) && y<Light_Bottom+2 && (abs_sub(Right[y],Light_X)<abs_sub(Light_X,Left[y])) && (abs_sub(Right[y],Right[y+1])<=Change_YuZhi) && Lost_R[y] == 0 && Right[y]<=Light_Right+3)
  {
    int Start = Right[y]+4;
    Last_R = Right[y] = Right_Init;//先恢复默认值，等找到后自会更新
    if(Img[y][Right_Init] == ImgWhite)
      Lost_R[y] = LostWhite;
    if(Img[y][Right_Init] == ImgBlack)
      Lost_R[y] = LostBlack;
    for(int x = Start;x<Right_Init;x++)
    {
      if(Img[y][x]==ImgBlack)//找到黑点
      {//进一步确认
        if(Img[y][x+3]==ImgBlack && Img[y][x+2]==ImgBlack && Img[y][x+1]==ImgBlack
           && Img[y][x-4]==ImgWhite  && Img[y][x-3]==ImgWhite && Img[y][x-2]==ImgWhite && Img[y][x-1]==ImgWhite)
        { 
          if(x>Light_Right+4)
          {
            Last_R = Right[y]=x-2;
            Lost_R[y]=0;        //丢线标志位清零
            break;        //退出循环 
          }
        }
      }  
    } 
    ErrorType = RightCorrect_3;        
  }  
  
  //前轮右轮内侧误判为边界
//  if(0)
  if(y<Light_Bottom+8 && y>Light_Top && Right[y]<Right_Init && Right[y]>Light_X)
  {
    uint8 flag = 0;
    uint8 flag2 = 0;
    for(uint8 x = Right[y];x>Right[y]-10;x--)
    {
      if(Img[y][x] == ImgBlack)
      {
        flag = 1;
        break;
      }
    }
    for(uint8 y1 = y;y1<y+5 && y1<119;y1++)
    {
      if(Img[y1][Right[y]] == ImgBlack)
      {
        flag2 = 1;
        break;
      }
    }    
    if(flag == 1 && flag2 ==1)
    {
      uint8 Num = 0;
      for(uint8 i = y;i<119;i++)
      {
        if(Lost_R[i] == LostWhite && Img[i][Right[i]] == ImgWhite)
          Num++;
      }
      if(Num>2 && abs_sub(Right[y],Right[y+2])>15)
      {
        Last_R = Right[y] = Right[y+1] = Right_Init;
        if(Img[y][Right_Init] == ImgWhite)
          Lost_R[y] = Lost_R[y+1] = LostWhite;
        if(Img[y][Right_Init] == ImgBlack)
          Lost_R[y] = Lost_R[y+1] = LostBlack;
        ErrorType = RightCorrect_4;     
      }
      else if(Num>8)
      {
        Last_R = Right[y] = Right_Init;
        if(Img[y][Right_Init] == ImgWhite)
          Lost_R[y] = LostWhite;
        if(Img[y][Right_Init] == ImgBlack)
          Lost_R[y] = LostBlack;
        ErrorType = RightCorrect_4;
      }          }
  } 
  //U型弯末端，找不到光源，只看到前车后轮
  if(RoadScan.Res.state != Exist || Light_X>140)
  {
    uint8 YuZhi_X = 120;
    uint8 YuZhi_Y = 59;
    uint8 YuZhi_Sight = 45;
    if(Light_X >YuZhi_X && Light_Y > YuZhi_Y && Sight_Last>YuZhi_Sight)
    {
      if(Right[y]<Right[y+1])
      {
        Last_R = Right[y] = Right[y+1];
        Lost_R[y+1] = Lost_R[y] = LostWhite;
        //y+1行也变成LostWhite，是有时候最初的一行能找到，但找的是车
        ErrorType = RightCorrect_5;
      }
    }  
  }
  //弯道末端，白点上方找错
  //if(0)
  if(Light_X>126)
  {
    if(Right[y]<Right[y+1])
    {
      Last_R = Right[y] = Right[y+1];
      Lost_R[y+1] = Lost_R[y] = LostWhite;
      //y+1行也变成LostWhite，是有时候最初的一行能找到，但找的是车
      ErrorType = RightCorrect_6;
    }
  }
}
//三角超车区中线计算
void Middle_Triangle()
{
  if(CarType == Later)
  {
    if(RoadType == Triangle_Left_GoIn)
    {
      uint8 Y_Start = 116;
      uint8 Y_Stop  = Sight;
      int middle    = 80;
      //初步寻找补线起始行
      for(uint8 y = 116;y>Sight;y--)
      {
        if(Lost_L[y] == LostWhite && Lost_R[y] == LostWhite)
        {
          Y_Start = y;
          break;
        }
      }
      //寻找补线结束行
      for(uint8 y = Y_Start;y>Sight;y--)
      {
        if(Lost_L[y] == 0 || Lost_R[y] == 0)
        {
          Sight_L = Sight_R = Sight = y;
          Y_Stop = y;
          break;
        }
      }
      //确定补线起始行
      for(uint8 y = Y_Start;y<116;y++)
      {
        if(Lost_L[y] == 0 && Lost_R[y] == 0)
        {
          Y_Start = y;
          break;
        }
      }
      //开始补线
      for(uint8 y = Y_Start;y>Y_Stop;y--)
      {
        middle = Middle[y+1]-(y-Y_Start)*(y-Y_Start)*ImgCurveDegree;
        if(middle<Left_Init)
          middle = Left_Init;
        Middle[y] = middle;
        Left[y] = Left_Init;
        Right[y] = Right_Init;
        Lost_L[y] = Lost_R[y] = 0;//为了让计算偏差时利用本行
      }   
    }
    if(RoadType == Triangle_Right_GoIn)
    {
      uint8 Y_Start = 116;
      uint8 Y_Stop  = Sight;
      int middle    = 80;
      //初步寻找补线起始行
      for(uint8 y = 116;y>Sight;y--)
      {
        if(Lost_L[y] == LostWhite && Lost_R[y] == LostWhite)
        {
          Y_Start = y;
          break;
        }
      }
      //寻找补线结束行
      for(uint8 y = Y_Start;y>Sight;y--)
      {
        if(Lost_L[y] == 0 || Lost_R[y] == 0)
        {
          Sight_L = Sight_R = Sight = y;
          Y_Stop = y;
          break;
        }
      }
      //确定补线起始行
      for(uint8 y = Y_Start;y<116;y++)
      {
        if(Lost_L[y] == 0 && Lost_R[y] == 0)
        {
          Y_Start = y;
          break;
        }
      }
      //开始补线
      for(uint8 y = Y_Start;y>Y_Stop;y--)
      {
        middle = Middle[y+1]+(y-Y_Start)*(y-Y_Start)*ImgCurveDegree;
        if(middle>Right_Init)
          middle = Right_Init;
        Middle[y] = middle;
        Left[y] = Left_Init;
        Right[y] = Right_Init;
        Lost_L[y] = Lost_R[y] = 0;//为了让计算偏差时利用本行
      }   
    }
  }
}
//出界检测
void OutsideCheck()
{
  if(CarType == Front && CarState == CarStart)
  {
    int BlackNum = 0;
    for(uint8 x = 0;x<CAMERA_W;x++)
    {
      if(Img[119][x] == ImgBlack)
        BlackNum++;
    }
    if(BlackNum>156)
    {
      CarState = CarStop;
    }
  }
}
void LCD_Find()
{
  if(CarType == Later) 
  {
    // 检测前车LCD
    RoadScan.Plan();  
    if(RoadScan.Res.state == Exist)
    {
      if((RoadType_AnotherCar == Overtake_Right || RoadType_AnotherCar == Overtake_Right_ing) && RoadScan.Res.x<70){}
      else if((RoadType_AnotherCar == Overtake_Left || RoadType_AnotherCar == Overtake_Left_ing) && RoadScan.Res.x>90){}  
      else 
      {
        Light_X =  RoadScan.Res.x;
        Light_Y =  RoadScan.Res.y;
        Light_Bottom = Light_Y+RoadScan.Res.High/2+1;
        Light_Top    = Light_Y-RoadScan.Res.High/2-1;
        Light_Left   = Light_X-RoadScan.Res.Wide/2-1;
        Light_Right  = Light_X+RoadScan.Res.Wide/2+1;
        if(Light_Left<0) Light_Left = 0;
        if(Light_Right>159) Light_Right = 159;
        if(Light_Top<Sight_Init) Light_Top = Sight_Init;
        if(Light_Bottom>110) Light_Bottom = 110;
      }
    }
  }
}
//void Triangle_Find()
//{
//  if(RoadScan.tRes == Exist)
//  {
//    RoadType = 
//  }
//}