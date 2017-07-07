/***********************************************************
老男孩之              图像滤波
***********************************************************/
#include "common.h"
#include "include.h"
#include  "Lvbo.h"
extern int Row;
float XieLv_R,XieLv_L;
int MostLeftInRight_X;    //右边界中最靠左边的
int MostLeftInRight_Y ;
int MostRightInLeft_X;    //左边界中最靠右边
int MostRightInLeft_Y ;
int RodNum = 0;
int RodRemove_Y_Start_L,RodRemove_Y_Stop_L,RodRemove_Y_Start_R,RodRemove_Y_Stop_R;
/****************************
作用：将出弯时断成两截的中线缝合起来
返回：无
****************************/
void Lvbo_Middle()
{
  int Error = 0;
  int YuZhi = 13;
  for(int y=100;y>Sight+16;y--)
  {
    Error =abs_sub(Middle[y],Middle[y-1]);
    if(Error>YuZhi)
    {//相邻两行之间中线偏差大于阈值，并且方向不同
      //先右后左的折线
      if((Middle[y+1]>=Middle[y+2] && Middle[y+2]>=Middle[y+3] && Middle[y+3]>=Middle[y+4] && Middle[y+4]>=Middle[y+5] && Middle[y+5]>=Middle[y+6] && Middle[y+6]>=Middle[y+7] && Middle[y+7]>=Middle[y+8] && Middle[y+8]>=Middle[y+9] && Middle[y+9]>=Middle[y+10] && Middle[y+10]>=Middle[y+11] && Middle[y+11]>=Middle[y+12] && Middle[y+12]>=Middle[y+13] && Middle[y+13]>=Middle[y+14] && Middle[y+14]>=Middle[y+15] && Middle[y+15]>=Middle[y+16] && Middle[y+16]>=Middle[y+17] && Middle[y+17]>=Middle[y+18] && Middle[y+18]>=Middle[y+19] && Middle[y+1]>Middle[y+19]) 
         &&(Middle[y-1]>=Middle[y-2] && Middle[y-2]>=Middle[y-3] && Middle[y-3]>=Middle[y-4] && Middle[y-4]>=Middle[y-5] && Middle[y-5]>=Middle[y-6] && Middle[y-6]>=Middle[y-7] && Middle[y-7]>=Middle[y-8] && Middle[y-8]>=Middle[y-9] && Middle[y-9]>=Middle[y-10] && Middle[y-10]>=Middle[y-11] && Middle[y-11]>=Middle[y-12] && Middle[y-12]>=Middle[y-13] && Middle[y-13]>=Middle[y-14] && Middle[y-14]>=Middle[y-15] && Middle[y-15]>=Middle[y-16] && Middle[y-1]>Middle[y-16]))
      {
        for(int i = y;(i<=y+25) && i<110;i++)
        {
          if(Middle[i]>Middle[y-1])
            Middle[i]=Middle[y-1];     //补成一条竖着的直线
        }
      }
      //先左后右的折线
      if((Middle[y+1]<=Middle[y+2] && Middle[y+2]<=Middle[y+3] && Middle[y+3]<=Middle[y+4] && Middle[y+4]<=Middle[y+5] && Middle[y+5]<=Middle[y+6] && Middle[y+6]<=Middle[y+7] && Middle[y+7]<=Middle[y+8] && Middle[y+8]<=Middle[y+9] && Middle[y+9]<=Middle[y+10] && Middle[y+10]<=Middle[y+11] && Middle[y+11]<=Middle[y+12] && Middle[y+12]<=Middle[y+13] && Middle[y+13]<=Middle[y+14] && Middle[y+14]<=Middle[y+15] && Middle[y+15]<=Middle[y+16] && Middle[y+16]<=Middle[y+17] && Middle[y+17]<=Middle[y+18] && Middle[y+18]<=Middle[y+19] && Middle[y+1]<Middle[y+19]) 
         &&(Middle[y-1]<=Middle[y-2] && Middle[y-2]<=Middle[y-3] && Middle[y-3]<=Middle[y-4] && Middle[y-4]<=Middle[y-5] && Middle[y-5]<=Middle[y-6] && Middle[y-6]<=Middle[y-7] && Middle[y-7]<=Middle[y-8] && Middle[y-8]<=Middle[y-9] && Middle[y-9]<=Middle[y-10] && Middle[y-10]<=Middle[y-11] && Middle[y-11]<=Middle[y-12] && Middle[y-12]<=Middle[y-13] && Middle[y-13]<=Middle[y-14] && Middle[y-14]<=Middle[y-15] && Middle[y-15]<=Middle[y-16] && Middle[y-1]<Middle[y-16]))
      {
        for(int i = y;(i<=y+25) && i<110;i++)
        {
          if(Middle[i]<Middle[y-1])
            Middle[i]=Middle[y-1];
        }
      }
    }
  }
}
/****************************
作用：出弯入直图像优化，减弱转过的情况
返回：无
****************************/
void Img_GoOutBend_Optimize()
{
  if(Distance_GoOutBend == 0 && RoadType == Normal)
  {
    
    if(Sight-Sight_Init_Turn<=6)//看的比较远
    {
      //左出弯
      int YuZhi = 15; 
      if((Lost_L_Num>YuZhi) && (Lost_R_Num<YuZhi) && Right[Sight]<80 && Left[119]<15)//左边丢线较多,右边丢线少
      {                                                                 //和大S区别开来
        uint8 Num = 0;
        for(uint8 y = Sight_Init_Turn;y<(Sight_Init_Turn+13);y++)
        {
          if(Middle[y] == Left_Init)
            Num++;
        }
        if(Num>9) // 远处中线都是补过头的
        {//重新修正中线
          RoadType = GoOutBend;   //将赛道类型修改为出弯
          Distance_GoOutBend = Distance;
          for(uint8 y =119;y>Sight_Init_Turn;y--)
          {
            Middle[y] = (Left[y]+Right[y])/2;
          }
        }
      }
      if((Lost_R_Num>YuZhi) && (Lost_L_Num<YuZhi) && Left[Sight]>80 && Right[119]>144)//左边丢线较少,右边丢线多
      {                                                               //和大S区别开来              
        uint8 Num = 0;
        for(uint8 y = Sight_Init_Turn;y<(Sight_Init_Turn+13);y++)
        {
          if(Middle[y] == Right_Init)
            Num++;
        }
        if(Num>9) // 远处中线都是补过头的
        {//重新修正中线
          RoadType = GoOutBend;   //将赛道类型修改为出弯
          Distance_GoOutBend = Distance;
          for(uint8 y =119;y>Sight_Init_Turn;y--)
          {
            Middle[y] = (Left[y]+Right[y])/2;
          }
        }
      }
    }
  }
  int YuZhi = 11;                //保证出弯优化能连续作用至少12cm的路程
  if(Distance_GoOutBend != 0 && Distance-Distance_GoOutBend<YuZhi)
  {
    RoadType = GoOutBend;   //将赛道类型修改为出弯
    for(uint8 y =119;y>Sight_Init_Turn;y--)
    {
      Middle[y] = (Left[y]+Right[y])/2;
    }    
  }
  if(Distance_GoOutBend != 0 && Distance-Distance_GoOutBend>=YuZhi)
  {
    RoadType = 0;
    Distance_GoOutBend = 0;
  }
}

int Lvbo_Middle2(uint8 y,int middle)
{
  int YuZhi = 10;
  uint8 Flag = 0;
  if((0==Lost_L[y-1]) && (0!=Lost_R[y-1]))     //左边有，右边没有
  { 
    Flag =1;
    Middle[y-1]=Middle[y]+(Left[y-1]-Left[y]);  //按照左边变化趋势补线
  }
  
  else if((0!=Lost_L[y-1]) && (0==Lost_R[y-1]))     //左边没有，右边有
  {  
    Flag=1;
    Middle[y-1]=Middle[y]-(Right[y]-Right[y-1]);  //按照右边变化趋势补线  
  }
  if(Flag == 1 && Middle[y]<Middle[y+1] && Middle[y]<Middle[y-1] && abs_sub(Middle[y],Middle[y+1])>YuZhi)
  {
    if(Lost_L[y] == LostWhite && Lost_R[y] == LostWhite)
    {
      if(Lost_R[y-1] == 0 && Lost_L[y-1] !=0)
      {
        middle=(Middle[y+3]+Middle[y+2]+Middle[y+1])/3;
        return middle;
      }
    }
  }
  if(Flag == 1 && Middle[y]>Middle[y+1] && Middle[y]>Middle[y-1] && abs_sub(Middle[y],Middle[y+1])>YuZhi)
  {
    if(Lost_L[y] == LostWhite && Lost_R[y] == LostWhite)
    {
      if(Lost_L[y-1] == 0 && Lost_R[y-1] !=0)
      {
        middle=(Middle[y+3]+Middle[y+2]+Middle[y+1])/3;
        return middle;
      }
    }  
  } 
  return middle;
}
void Lvbo_Left(uint8 y)
{
  uint8 YuZhi=20;
  if(abs_sub(Left[y],Left[y+1])>YuZhi)
  {
    Last_L=Left[y]=Left[y+1];
    if(Img[y][Left[y]]==ImgWhite)
    {
      Lost_L[y] = LostWhite;
    }
    if(Img[y][Left[y]]==ImgBlack)
    {
      Lost_L[y] = LostBlack;
    }    
  }
}
void Lvbo_Right(uint8 y)
{
  uint8 YuZhi=20;
  if(abs_sub(Right[y],Right[y+1])>YuZhi)
  {
    Last_R=Right[y]=Right[y+1];
    if(Img[y][Right[y]]==ImgWhite)
    {
      Lost_R[y] = LostWhite;
    }
    if(Img[y][Right[y]]==ImgBlack)
    {
      Lost_R[y] = LostBlack;
    }    
  }
}
//前车内切，和边界混在一起
void Middle_Correct()
{
  if(CarType == Later)
  {
    //if(Light_Y>70)
    {
    XieLv_R = 0;
    XieLv_L = 0;
    //    int YuZhi_L     = 80;       //我尼玛，限制越改越小，都快变成象征性的限制了
    //    int YuZhi_R     = 70;
    int YuZhi_X     = 22;       //如果边界线中某一行和底部差了这么多，那么可能就是将车身当成赛道了
    float YuZhi_XieLv_L = 1.0; //绝对值越大，限制越严格
    float YuZhi_XieLv_R = -1.0;//绝对值越大，限制越严格
    int XieLv_Y_Stop    = 12; 
    int XieLv_Y_Pianyi  = 3;    //为求得更准确的斜率
    MostLeftInRight_X = Right[119];    //右边界中最靠左边的
    MostLeftInRight_Y = 116;             //右边界中最靠左边的
    MostRightInLeft_X = Left[119];    //左边界中最靠右边
    MostRightInLeft_Y = 116;
    for(Row = 116;Row>Light_Bottom;Row--)
    {
      if(Right[Row]<MostLeftInRight_X)
      {
        MostLeftInRight_X = Right[Row];
        MostLeftInRight_Y = Row;
      }
      if(Left[Row]>MostRightInLeft_X)
      {
        MostRightInLeft_X = Left[Row];
        MostRightInLeft_Y = Row;
      }
    }
    // if(Light_X>Right[Light_Bottom+2])
    if((abs_sub(Right[MostLeftInRight_Y],Light_X)<abs_sub(Left[MostLeftInRight_Y],Light_X)) )
    {
      for(Row = 116;Row>MostLeftInRight_Y;Row--)
      {
        uint8 Black_Num = 0;
        for(uint8 x = Left[Row];x<Right[Row] && Img[Row][Right[Row]] == ImgWhite;x+=5)//右转时有时候左边界斜率也很大，但是左边界和右边界之间有车
        {
          if(Img[Row][x] == ImgBlack)
            Black_Num++;
          if(Black_Num >2)
          {
            return;
          }
        }
      }
      int temp_y = MostLeftInRight_Y+XieLv_Y_Stop+XieLv_Y_Pianyi;
      if(MostLeftInRight_Y+XieLv_Y_Stop+XieLv_Y_Pianyi>116)
      {
        temp_y = 116;
      }
      XieLv_R = (float)((int)Right[MostLeftInRight_Y+XieLv_Y_Pianyi]-(int)Right[temp_y])/(float)(temp_y - (MostLeftInRight_Y+XieLv_Y_Pianyi));
      if(XieLv_R<=YuZhi_XieLv_R)
      {
        
        memset(Lost_R+Light_Y,0,119-Light_Y);//尽量平衡矛盾，否则可能计算偏差时出现底部大批数据没有用到，在Window临界点处会抖动
        GenZong_Again_Right();
        jisuan_Middle();  
        ErrorType = MiddleCorrect;//记录出错原因
      }
    }
    
    // if(Light_X<Left[Light_Bottom+2])
    if(abs_sub(Right[MostRightInLeft_Y],Light_X)>abs_sub(Left[MostRightInLeft_Y],Light_X))
    {
      uint8 flag      = 0;
      for(Row = 116;Row>MostRightInLeft_Y;Row--)
      {
        uint8 Black_Num = 0;
        //  if(Row> Light_Bottom +10 && Img[Row][Left[Row] == ImgWhite])
        {
          for(uint8 x = Left[Row];x<Right[Row] && Img[Row][Right[Row]] == ImgWhite ;x+=5)//右转时有时候左边界斜率也很大，但是左边界和右边界之间有车
          {
            if(Img[Row][x] == ImgBlack)
            {
              Black_Num++;
            }
            if(Black_Num >2)
            {
              return;
            }
          }
        }
      }
      
      int temp_y = MostRightInLeft_Y+XieLv_Y_Stop+XieLv_Y_Pianyi;
      if(MostRightInLeft_Y+XieLv_Y_Stop+XieLv_Y_Pianyi>116)
        temp_y = 116;
      //        float XieLv_L = (float)(Left[MostRightInLeft_Y+XieLv_Y_Pianyi]-Left[MostRightInLeft_Y+XieLv_Y_Stop+XieLv_Y_Pianyi])/(float)(temp_y - (MostRightInLeft_Y+XieLv_Y_Pianyi));
      XieLv_L = (float)((int)Left[MostRightInLeft_Y+XieLv_Y_Pianyi]-(int)Left[temp_y])/(float)(temp_y - (MostRightInLeft_Y+XieLv_Y_Pianyi));
      
      if(XieLv_L>=YuZhi_XieLv_L)
      {
        memset(Lost_L+Light_Y,0,119-Light_Y);//尽量平衡矛盾，否则可能计算偏差时出现底部大批数据没有用到，在Window临界点处会抖动
        GenZong_Again_Left();
        jisuan_Middle(); 
        ErrorType = MiddleCorrect;//记录出错原因
      }
    }
    
    }
  }
}
void Middle_Correct2()
{
  uint8 Num = 0;
  if(Img[117][Left[117]] == ImgBlack && Right[117]>136)
  {
    for(Row = Light_Bottom+8;Row>50;Row--)
    {
      if(Lost_L[Row] == 0 && Lost_R[Row] == 0 && abs_sub(Middle[Row],Middle[Row+1])>2)
      {
        for(uint8 y = Row;y<Row+15 && y<117;y++)
        {
          if(Lost_L[y] !=0 && (Lost_R[y] == 0 || Lost_R[y] == LostWhite))
          {
            Num++;
          }
          if(Img[y][Left[y+1]] == ImgWhite && Left[y+1]<10 && Left[y+1] == Left[y+2])
          {//大s
            return;
          }
        }
        if(Num >0)
        {
          int middle,left;
          for(uint8 y = Row;y<119;y++)
          {
            middle = Middle[y-1]+abs_sub(Right[y],Right[y-1]);
            if(middle<Left_Init) middle = Left_Init;
            if(middle>Right_Init) middle = Right_Init;
            left   = middle - abs_sub(Right[y],middle);
            if(left<Left_Init) left = Left_Init;
            Middle[y] = middle; Left[y] = left;
            Lost_L[y] = 0;
          }
          ErrorType = MiddleCorrect2;
          break;
        }
      }
    }
  }
  Num = 0;
  if(Img[117][Right[117]] == ImgBlack  && Left[117]<24)
  {
    for(Row = Light_Bottom+6;Row>50;Row--)
    {
      if(Lost_L[Row] == 0 && Lost_R[Row] == 0 && abs_sub(Middle[Row],Middle[Row+1])>2)
      {
        for(uint8 y = Row;y<Row+15 && y<117;y++)
        {
          if(Lost_R[y] == LostBlack && Lost_L[y] == 0)
          {
            Num++;
          }
          if(Img[y][Right[y+1]] == ImgWhite && Right[y+1]>149 && Right[y+1] == Right[y+2])
          {//大s
            return;
          }         
          
        }
        if(Num >0)
        {
          int middle,right;
          for(uint8 y = Row;y<119;y++)
          {
            middle = Middle[y-1]-abs_sub(Left[y],Left[y-1]);
            if(middle<Left_Init) middle = Left_Init;
            if(middle>Right_Init) middle = Right_Init;
            right   = middle + abs_sub(middle,Left[y]);
            if(right<Right_Init) right = Right_Init;
            Middle[y] = middle; Right[y] = right;
            Lost_R[y] = 0;
          }
          ErrorType = MiddleCorrect2;
          break;
        }
      }
    }
  }
}
void Overtake_Return_GenZong()
{
  int Window = 11;
  //右超车区
  if(RoadType == Overtake_Right_Rerturn_Execute)
  {
    Sight = Sight_L = Sight_Init;
    Last_L = Left[116];
    memset(Lost_L,0,CAMERA_H);
    memset(Lost_R,0,CAMERA_H);
    memset(Left,Left_Init,CAMERA_H);
    
    if(RoadScan.Res.state != Exist)
    {
      Light_X = 14;
      Light_Y = 74;             //或许会引起速度控制的抖动
      Light_Bottom = Light_Y+4;
      Light_Top    = Light_Y-4;
      Light_Left   = Light_X-4;
      Light_Right  = Light_X+4;
    }
    
    uint8 y,x;
    uint8 Final_L=Left_Init,Start_L;
    
    for(y = 116;y>Sight_L;y--)
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
          if(Img[y][x-1]==ImgBlack && Img[y][x-2]==ImgBlack && Img[y][x+1]==ImgWhite && Img[y][x+2]==ImgWhite  && Img[y][x+3]==ImgWhite)
          {    
            Left[y]=x+2;
            Lost_L[y]=0;        //丢线标志位清零
            if(y>Light_Top && Left[y]>Light_Left-2)
            {
              Left[y]=Left_Init;
              Lost_L[y]=LostWhite;               
            }
            break;        //退出循环 
          }
        }           
      }
      Last_L=Left[y];     //保存上一次的值
      if(Left[y]==Left_Init)//说明没有提取到边界
      {
        Last_L=Left[y]=Left[y+1];
      }
    }
    for(y = 119;y>Sight_L;y--)
    {
      Middle[y] = fanjiaozheng_x(y,Left[y],ROAD_HALF);
      Right[y]  = fanjiaozheng_x(y,Left[y],ROAD_W);
    }
  }
  
  //左超车区
  if(RoadType == Overtake_Left_Rerturn_Execute)
  {
    Sight = Sight_R = Sight_Init;
    Last_R = Right[116];
    memset(Lost_L,0,CAMERA_H);
    memset(Lost_R,0,CAMERA_H);
    memset(Right,Right_Init,CAMERA_H);
    
    if(RoadScan.Res.state != Exist)
    {
      Light_X = 159-14;
      Light_Y = 74;             //或许会引起速度控制的抖动
      Light_Bottom = Light_Y+4;
      Light_Top    = Light_Y-4;
      Light_Left   = Light_X-4;
      Light_Right  = Light_X+4;
    }
    uint8 y,x;
    uint8 Final_R=Right_Init,Start_R;
    
    for(y = 116;y>Sight_R;y--)
    {
      //左边界从右向左搜 
      if(Last_R+Window<Right_Init)
        Final_R=Last_R+Window;
      else
        Final_R=Right_Init;
      if(Last_R-Window>Left_Init)
        Start_R=Last_R-Window;
      else
        Start_R=Left_Init;
      for(x=Start_R;x<Final_R;x++)     
      {
        if(Img[y][x]==ImgBlack)//找到黑点
        {//进一步确认
          if(Img[y][x+1]==ImgBlack && Img[y][x+2]==ImgBlack && Img[y][x-1]==ImgWhite && Img[y][x-2]==ImgWhite  && Img[y][x-3]==ImgWhite)
          {    
            Right[y]=x-2;
            Lost_R[y]=0;        //丢线标志位清零
            if(y>Light_Top && Right[y]<Light_Right+2)
            {
              Right[y]=Right_Init;
              Lost_R[y]=LostWhite;               
            }
            break;        //退出循环 
          }
        }           
      }
      Last_R=Right[y];     //保存上一次的值
      if(Right[y]==Right_Init)//说明没有提取到边界
      {
        Last_R=Right[y]=Right[y+1];
      }
    }
    for(y = 119;y>Sight_R;y--)
    {
      Middle[y] = fanjiaozheng_x(y,Right[y],-ROAD_HALF);
      Left[y]  = fanjiaozheng_x(y,Right[y],-ROAD_W);
    }
  }
}
//去除前车摄像头支架的影响
void RodRemove()
{
  if(CarType == Later)
  {
    uint8 Num_R = 0;
    uint8 Num_L = 0;
    RodNum = 0;
    uint8 window = 12;
    uint8 YuZhi = 8;
    uint8 YuZhi_R = 15;
    uint8 YuZhi_L = 144;
    uint8 Flag_Find = 0;
    RodRemove_Y_Start_R =RodRemove_Y_Start_L= Light_Top-22;
    
    for(Row = Light_Top-22;Row>Sight && Row>31;Row--)
    {                                   //远方太乱
      if(Img[Row-1][Right[Row]] == ImgWhite && Lost_R[Row] == 0)
      {
        Num_R++;
        if(Num_R == 1)
          RodRemove_Y_Start_R = Row;
      }
      else
        Num_R = 0;
      if(Img[Row-1][Left[Row]] == ImgWhite && Lost_L[Row] == 0)
      {
        Num_L++;
        if(Num_L == 1)
          RodRemove_Y_Start_L = Row;
      }
      else
        Num_L = 0;
      if(Num_L>4)
      {
        RodNum = 0;
        RodRemove_Y_Stop_L = RodRemove_Y_Start_L -10;
        if(RodRemove_Y_Stop_L<Sight)
          RodRemove_Y_Stop_L = Sight;
        for(uint8 y = RodRemove_Y_Start_L;y>RodRemove_Y_Stop_L;y--)
        {
          int Final_L = Left[y]+window;
          if(Final_L >Right_Init) Final_L = Right_Init;
          for(uint8 x = Left[y];x<Final_L;x++)
          {
            if(Img[y][x] == ImgBlack && Img[y][x-1] == ImgWhite)
            {
              if(abs_sub(x,Left[y])<YuZhi)
                RodNum++;
              break;
            }
          }
        }
        if(RodNum>4)
        {
          ErrorType = Rod_L;
          Row = Sight;
        }
        else
        {
          Num_L = RodNum = 0;
        }
      } 
      if(Num_R>4)
      {
        RodNum = 0;
        RodRemove_Y_Stop_R = RodRemove_Y_Start_R -10;
        if(RodRemove_Y_Stop_R<Sight)
          RodRemove_Y_Stop_R = Sight;
        for(uint8 y = RodRemove_Y_Start_R;y>RodRemove_Y_Stop_R;y--)
        {
          int Final_R = Right[y]-window;
          if(Final_R <Left_Init) Final_R = Left_Init;
          for(uint8 x = Right[y];x>Final_R;x--)
          {
            if(Img[y][x] == ImgBlack && Img[y][x+1] == ImgWhite)
            {
              RodNum++;
              break;
            }
          }
        }
        if(RodNum>4)
        {
          ErrorType = Rod_R;
          Row = Sight;
        }
      }
    }

    

    if(ErrorType == Rod_R)
    {
      int Data = abs_sub(Right[RodRemove_Y_Start_R+1],Right[RodRemove_Y_Start_R+2]);
      if(Data<4) Data = 4;
      Last_R = Right[RodRemove_Y_Start_R]-Data;
      if(Last_R<Left_Init) Last_R = Left_Init;
      Right[RodRemove_Y_Start_R] = Last_R;
      uint8 Window = 14;
      uint8 RightCopy;
      uint8 Final_R,Start_R;
      int x;
      for(uint8 y = RodRemove_Y_Start_R-1;y>Sight;y--)
      {
        if(y<Sight_L)
        {
          Sight = Sight_R = Sight_L;
          return;
        }
        RightCopy = Right[y];
        Final_R=Right[y]-2;
        if(Last_R-Window>Left_Init)
          Start_R=Last_R-Window;
        else
          Start_R=Left_Init;
        //右边界从左向右搜 
        for(x=Start_R;x<Final_R;x++) 
        {
          if(Img[y][x]==ImgBlack)//找到黑点
          {//进一步确认
            if(Img[y][x-1]==ImgWhite && Img[y][x-2]==ImgWhite)
            { 
              Last_R = Right[y] = x-2;
              Lost_R[y]=0;
              break;//退出循环
            }
          }           
        }
        if(Right[y] == RightCopy)//没找到
        {
          Data = abs_sub(Right[y+1],Right[y+2]);
          if(Data<4) Data = 4;
          Last_R = Last_R-Data;
          if(Last_R <Left_Init)
          {
            Last_R = Left_Init;
            Right[y] = Last_R;
            Sight_R = Sight = y;
            return ;
          }
          else
            Right[y] = Last_R;
        }
        if(Right[y]<YuZhi_R)
        {
          Sight_R = Sight = y;
          return;
        }
        if(Right[y]<Left[y])
        {
          Sight_R = Sight_L = Sight = y+5;
          break;
        }
      }
      
      jisuan_Middle();
    }
    if(ErrorType == Rod_L)
    {
      int Data = abs_sub(Left[RodRemove_Y_Start_L+1],Left[RodRemove_Y_Start_L+2]);
      if(Data<4) Data = 4;
      Last_L = Left[RodRemove_Y_Start_L]+Data;
      if(Last_L>Right_Init) Last_L = Right_Init;
      Left[RodRemove_Y_Start_L] = Last_L;
      uint8 Window = 14;
      uint8 LeftCopy;
      uint8 Final_L,Start_L;
      int x;
      for(uint8 y = RodRemove_Y_Start_L-1;y>Sight;y--)
      {
        if(y<Sight_R)
        {
          Sight = Sight_L = Sight_R;
          return;
        }
        LeftCopy = Left[y];
        Flag_Find = 0;
        Final_L=Left[y]+2;
        if(Last_L+Window<Right_Init)
          Start_L=Last_L+Window;
        else
          Start_L=Right_Init;
        //右边界从左向右搜 
        for(x=Start_L;x>Final_L;x--) 
        {
          if(Img[y][x]==ImgWhite)//找到黑点
          {//进一步确认
            if(Img[y][x-1]==ImgBlack && Img[y][x-2]==ImgBlack)
            { 
              Last_L = Left[y] = x+1;
              Lost_L[y]=0;
              Flag_Find = 1;
              break;//退出循环
            }
          }           
        }
        if(Flag_Find == 0)//没找到
        {
          Data = abs_sub(Left[y+1],Left[y+2]);
          if(Data<4) Data = 4;
          Last_L = Last_L+Data;
          if(Last_L >Right_Init)
          {
            Last_L = Right_Init;
            Left[y] = Last_L;
            Sight_L = Sight_R = Sight = y;
            return ;
          }
          else
            Left[y] = Last_L;
        }
        if(Left[y]>YuZhi_L)
        {
          Sight_L = Sight_R = Sight = y;
          return;
        }
        if(Right[y]<Left[y])
        {
          Sight_R = Sight_L = Sight = y+5;
          break;
        }
      }
      jisuan_Middle();
    }
  }
}