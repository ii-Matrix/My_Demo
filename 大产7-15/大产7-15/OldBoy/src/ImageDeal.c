/***********************************************************
���к�֮                      ͼ����
***********************************************************/
#include "common.h"
#include "include.h"
#include  "Var.h"
#include  "ImageDeal.h"
#define CurveTypeRecordMax 20
uint8 CurveType_History[CurveTypeRecordMax];
uint8 ImgBufferAgo[CAMERA_SIZE+360+(VarReceiveMax+VarSendMax)*3+10],ImgBufferNow[CAMERA_SIZE+360+(VarReceiveMax+VarSendMax)*3+10];//ԭʼѹ��ͼ��洢������
uint8 Img[CAMERA_H][CAMERA_W];           //��ѹ�����ͼ��
uint8 *ImgNow,*ImgAgo;
//uint8 SelectRow[RowMax];              //����ƫ��ʱ���õ��У��Ⱦ���ָ�������ͼ��������������
//uint8 SelectRowFlag[CAMERA_H];          //����ƫ�������б�־λ��1�������������
float Img_P[12];                        //ͼ��Ȩ�رȣ�ÿ10��һ��Ȩ�ر�
float a2,b2,c2,d2,e2,f2,v2;                //��У��ϵ��
uint8 Left[CAMERA_H],Middle[CAMERA_H],Right[CAMERA_H];//��,�У�����ֵ
int Last_L,Last_R;                    //��һ�еı߽�ֵ
uint8 Jizhun_L[3],Jizhun_R[3];          //��׼����
uint8 Lost_L[CAMERA_H],Lost_R[CAMERA_H];        //�������
//int Left_Init=3,Right_Init=156,Middle_Init=80;           //�����ҳ�ʼֵ����ֹ����ʱ����Խ��
int Left_Init=3,Right_Init=146,Middle_Init=69;           //�����ҳ�ʼֵ����ֹ����ʱ����Խ��
int Sight_Last,Sight=119,Sight_L=24,Sight_R=24,Sight_Init=24,Sight_L_Init=24,Sight_R_Init=24;//ǰհ����ʼ��
int Sight_Init_Turn  = 26;
Road_Type RoadType,RoadType_Last,RoadType_AnotherCar;                 //��������
uint8 Lost_L_Num,Lost_R_Num;    //�����ۼ�
uint8 ValidLine;                //��Ч����
uint32 ImgNum_GoInBend;         //����ͼƬ��¼
uint32 Img_GoOutBend;           //����ͼƬ��¼
uint32 Flag_GoInBend;           //ֱ�������־λ
uint32 Flag_GoOutBend;          //ֱ�߳����־λ
uint32 ImgNum;                  //ͼƬ��Ŀ
uint32 ImgStopNum;              //ֹͣʱ��ͼƬID
uint32 StraightNum;             //����ֱ������Ŀ
uint32 BendNum;                 //�����������Ŀ
uint32 Img_GoOutBend;            //����ʱ��ͼƬ��Ŀ
uint8  CrossNum;                //ʮ����Ŀ
uint8  LightLimtY_BottomFind = 78;  //�󳵵ײ�Ѱ��ʱ��ǰ����Դ����������ֵʱ��������һ�ַ���Ѱ��
int Light_X ,Light_Y,Light_Bottom,Light_Top,Light_Left,Light_Right;          //ǰ����Դ���꣬���,���                                         //ǰ�����
float MiddleError,MiddleError_Last,MiddleError_Previous;              //����ͼ��ƫ��,�ϳ������ϳ�
float AbsMiddleError;           //ͼ��ƫ��ľ���ֵ
float ImgCurveDegree;           //���������̶�ϵ��
float ;           //��ʮ�ּ�¼
Error_Type ErrorType;           //ͼ�����ԭ���¼
Curve_Type CurveType;           //�����̶ȣ���ҪΪ���ٶȿ��Ʒ���
int Row;
float Cross_Xielv,Cross_Xielv2;
int Y_Stop_GenZongAgain;
uint8 Y_Overtake_Start;
float XieLv_Overtake_Middle,XieLv_Overtake_Left,XieLv_Overtake_Right;
int Road_OvetakeFind_X;
/****************************
���ã�ͼ��ײ�Ѱ�ң�ȷ����׼ֵ
****************************/
void BottomFind()
{   
  int x=0,y=0;                          //yΪ�����꣬xΪ������
  int BlackNum_L=0,BlackNum_R=0;        //�ײ��������Һڵ�����
  int Start_L = 80;
  int Start_R = 80 ;                      //�ײ�������ʼ��
  if(CarType == Front)                  //ǰ��Ѱ��
  {
    for(y=119;y>116;y--)                  //ͳ�����Һڵ���Ŀ������>�ң����Կ��ҿ�ʼ����
    {
      for(int x = 79;x>4;x-=4)            //-4��Ϊ�˼ӿ��ٶȣ������ж�û��Ҫ����ȷ
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
      //���һ�׼
      for(x=Start_R;x<Right_Init;x++)
      {   //Ѱ�������
        if(Img[y][x+1]==ImgBlack && Img[y][x+2]==ImgBlack && Img[y][x]==ImgWhite && Img[y][x-1]==ImgWhite && Img[y][x-2]==ImgWhite && Img[y][x-3]==ImgWhite)
        {
          Right[y]=x-1;             //��¼
          Lost_R[y]=0;            //���߱�־λ����
          break;
        }
      }
      
      //�����׼
      for(x=Start_L;x>Left_Init;x--)
      {
        //Ѱ�������
        if(Img[y][x-1]==ImgBlack && Img[y][x-2]==ImgBlack && Img[y][x]==ImgWhite && Img[y][x+1]==ImgWhite && Img[y][x+2]==ImgWhite && Img[y][x+3]==ImgWhite)
        {
          Left[y]=x+1;             //��¼
          Lost_L[y]=0;            //���߱�־λ����
          break;
        }
      }
      Jizhun_R[119-y]=Right[y];
      Jizhun_L[119-y]=Left[y];
      Middle[y]=(Right[y]+Left[y])/2;
      //   lvbo_Jizhun(y);
    }                       
    //�����׼��ֵ
    Last_L=(Jizhun_L[0]+Jizhun_L[1]+Jizhun_L[2])/3;
    Last_R=(Jizhun_R[0]+Jizhun_R[1]+Jizhun_R[2])/3;
  }
  if(CarType == Later)                          //��Ѱ��
  {
    
    //    else
    //      CarType = Front;
    if(Light_Y<LightLimtY_BottomFind)
    {
      for(y=119;y>116;y--)                  //ͳ�����Һڵ���Ŀ������>�ң����Կ��ҿ�ʼ����
      {
        for(int x = 79;x>4;x-=4)            //-4��Ϊ�˼ӿ��ٶȣ������ж�û��Ҫ����ȷ
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
      //���һ�׼
      for(x=Start_R;x<Right_Init;x++)
      {   //Ѱ�������
        if(Img[y][x]==ImgBlack && Img[y][x+1]==ImgBlack && Img[y][x-1]==ImgWhite && Img[y][x-2]==ImgWhite)
        {
          if(Img[y][x-12] == ImgWhite) // ��������ʱ���ܽ������ڲ��ҳɱ߽�
          {
            Right[y]=x-1;             //��¼
            Lost_R[y]=0;            //���߱�־λ����
            break;
          }
        }
      }
      
      //�����׼
      for(x=Start_L;x>Left_Init;x--)
      {
        //Ѱ�������
        if(Img[y][x]==ImgBlack && Img[y][x-1]==ImgBlack && Img[y][x+1]==ImgWhite && Img[y][x+2]==ImgWhite)
        {
          if(Img[y][x+12] == ImgWhite) // ��������ʱ���ܽ������ڲ��ҳɱ߽�
          {
            Left[y]=x+1;             //��¼
            Lost_L[y]=0;            //���߱�־λ����
            break;
          }
        }
      }
      if(y<119 && Light_Y>=LightLimtY_BottomFind)
      {//����̫�������
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
                    x = 80;           //����ѭ��
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
                    x = 80;           //����ѭ��
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
    //�����׼��ֵ
    Last_L=Left[117];
    Last_R=Right[117]; 
  }
}


/****************************
���ã��ҵ��߽���
****************************/
void GenZong()
{
  int x=0,y=0;
  int Window = 10;
  if(CarType == Front)                  //ǰ���ұ߽�
  {
    uint8 Final_L=Left_Init,Final_R=Right_Init,Start_L,Start_R;
    
    //һ·���ٵ���Чǰհ���¸�
    for(y=116;y>=Sight_Init;y--)// && Sight_L==Sight_L_Init && Sight_R==Sight_R_Init;y--)//�ж�����ô������Ϊ��ʱ��Ҫ�ı�Sight֮���ֵ����������
    {
      // if(Sight_L==Sight_L_Init)
      {
        //��߽���������� 
        if(Last_L-Window>Left_Init)
          Final_L=Last_L-Window;       
        if(Last_L+Window<Right_Init)
          Start_L=Last_L+Window;
        else
          Start_L=Right_Init;
        for(x=Start_L;x>Final_L;x--)     
        {
          if(Img[y][x]==ImgBlack)//�ҵ��ڵ�
          {//��һ��ȷ��
            if(Img[y][x-1]==ImgBlack && Img[y][x-2]==ImgBlack && Img[y][x+1]==ImgWhite && Img[y][x+2]==ImgWhite)
            { 
              Left[y]=x+2;
              Lost_L[y]=0;        //���߱�־λ����
              break;        //�˳�ѭ�� 
            }
          }           
        }
        Last_L=Left[y];     //������һ�ε�ֵ
        if(Left[y]==Left_Init)//˵��û����ȡ���߽�
        {
          Last_L=Left[y]=Refind_L(y);     //�ٴ�Ѱ��
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
        //�ұ߽���������� 
        for(x=Start_R;x<Final_R;x++) 
        {
          if(Img[y][x]==ImgBlack)//�ҵ��ڵ�
          {//��һ��ȷ��
            if(Img[y][x+1]==ImgBlack && Img[y][x+2]==ImgBlack && Img[y][x-1]==ImgWhite && Img[y][x-2]==ImgWhite)
            { 
              Right[y]=x-2;
              Lost_R[y]=0;
              break;//�˳�ѭ��
            }
          }           
        }
        Last_R=Right[y];     //������һ�ε�ֵ
        if(Right[y]==Right_Init)//˵���ұ߽�û����ȡ��
        {
          Last_R=Right[y]=Refind_R(y);    
        }
      }
    }
  }

}
/****************************
���ã���������
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
      if(Lost_L[y]==0 && Lost_R[y]==0)      //���߶����ҵ�
      {
        middle=(Left[y]+Right[y])/2;  
      }
      else if((0!=Lost_L[y]) && (0!=Lost_R[y]))     //���߶�û��
      {
        if((Lost_L[y]==LostWhite) && (Lost_R[y]==LostWhite))     //���߶�û��,���Ҷ����ߣ�������ʮ��
        {
          middle=(Middle[y+1]+Middle[y+2]+Middle[y+3])/3;       //�е����ǰ���о�ֵ,����ƫ��ʱ���в�����
          //  if(CarState == CarStop)//̫��ʱ�������ʱ��
          {
            Last_L=Left[y]=fanjiaozheng_x(y,middle,-ROAD_HALF);   //ƽ���������һ�벹�߽���
            Last_R=Right[y]=fanjiaozheng_x(y,middle,+ROAD_HALF);
          }       
        }
        else
          middle=(Middle[y+3]+Middle[y+2]+Middle[y+1])/3;
      }
      else if((0==Lost_L[y]) && (0!=Lost_R[y]))     //����У��ұ�û��
      { 
        Lost_R_Num++;
        if(Lost_R[y+1] ==LostWhite && Lost_L[y+1] == LostWhite)
          middle = Middle[y+1];
        else       
          middle=Middle[y+1]+(Left[y]-Left[y+1]);//+(int)(Lost_R_Num*Lost_R_Num*ImgCurveDegree);  //������߱仯���Ʋ���
      }
      
      else if((0!=Lost_L[y]) && (0==Lost_R[y]))     //���û�У��ұ���
      { 
        Lost_L_Num++;
        if(Lost_R[y+1] ==LostWhite && Lost_L[y+1] == LostWhite)
          middle = Middle[y+1];
        else
          middle=Middle[y+1]-(Right[y+1]-Right[y]);//-(int)(Lost_L_Num*Lost_L_Num*ImgCurveDegree);  //�����ұ߱仯���Ʋ���   
      }
    }

    if(middle>Right_Init)
      middle=Right_Init;
    if(middle<Left_Init)
      middle=Left_Init;
    Middle[y]=middle;
  }
  //  Lvbo_Middle();//���
  
  
}
/****************************
���ã�δ�ҵ��߽���ʱ�ٴ�Ѱ��
��������ǰ�е�������
���أ��ҵ��ı߽��ߺ�����
****************************/
uint8 Refind_L(uint8 y)//����һ����߽�
{
  int Final,Start,x;            //�������ֳ���һ���������͵Ŀ���������uint8��������������ܱ�159��
  if(CarType == Front)
  {
    if(RoadType!=Overtake_Right_ing && RoadType!= Overtake_Left_ing)
    {
    uint8 Final_L=Left_Init,Final_R=Right_Init,Start_L,Start_R;
    int window = 22;
    //��߽���������� 
    if((int)Left[y+1]-window>Left_Init)
      Final_L=Left[y+1]-window;       
    if(Left[y+1]+window<Right_Init)
      Start_L=Left[y+1]+window;
    else
      Start_L=Right_Init;
    for(x=Start_L;x>Final_L;x--)     
    {
      if(Img[y][x]==ImgBlack)//�ҵ��ڵ�
      {//��һ��ȷ��
        if(Img[y][x-1]==ImgBlack && Img[y][x-2]==ImgBlack && Img[y][x+1]==ImgWhite && Img[y][x+2]==ImgWhite)
        { 
          Lost_L[y]=0;        //���߱�־λ����
          return x+2;  
        }
      } }          
    }
    //�����Ҳ���������������ɢ����
    if(Left[y+1]<78 && Right[y+1]>82  && RoadType!=Overtake_Right_ing && RoadType!= Overtake_Left_ing)      //������ɢ����������������ʮ��
    {
      for(x=80;x>Left_Init;x--)     
      {
        if(Img[y][x]==ImgBlack)//�ҵ��ڵ�
        {//��һ��ȷ��
          if(Img[y][x-1]==ImgBlack && Img[y][x-2]==ImgBlack && Img[y][x+1]==ImgWhite && Img[y][x+2]==ImgWhite && Img[y][x+3]==ImgWhite)
          { 
            Lost_L[y]=0;        //���߱�־λ����
            return x+2;            
          }           
        }
      }
    } 
    if(Img[y][Left[y+1]]==ImgWhite )//�����ǰ�ɫ                          
      Lost_L[y]=LostWhite;
    if(Img[y][Left[y+1]]==ImgBlack)//�����Ǻ�ɫ
      Lost_L[y]=LostBlack;
    return Left[y+1];//����û�ҵ���������һ�е�ֵ
  }

}
/****************************
���ã�δ�ҵ��߽���ʱ�ٴ�Ѱ��
��������ǰ�е�������
���أ��ҵ��ı߽��ߺ�����
****************************/
uint8 Refind_R(uint8 y)         //����һ���ұ߽�
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
    //�ұ߽���������� 
    for(x=Start_R;x<Final_R;x++) 
    {
      if(Img[y][x]==ImgBlack)//�ҵ��ڵ�
      {//��һ��ȷ��
        if(Img[y][x+1]==ImgBlack && Img[y][x+2]==ImgBlack && Img[y][x-1]==ImgWhite && Img[y][x-2]==ImgWhite)
        { 
          Lost_R[y]=0;        //���߱�־λ����
          return x-2; 
        }
      }           
    }
    }
    if(Left[y+1]<78 && Right[y+1]>82  && RoadType!=Overtake_Right_ing && RoadType!= Overtake_Left_ing)  
      //�����Ҳ���������������ɢ����
    {
      for(int x=80;x<Right_Init;x++)
      {
        if(Img[y][x]==ImgBlack)//�ҵ��ڵ�
        {//��һ��ȷ��
          if(Img[y][x+1]==ImgBlack && Img[y][x+2]==ImgBlack && Img[y][x-1]==ImgWhite && Img[y][x-2]==ImgWhite && Img[y][x-3]==ImgWhite)
          { 
            Lost_R[y]=0;        //���߱�־λ����
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
���ã���̬ǰհ��ȷ��
****************************/
void ForeSight()
{
  if(Sight_L==Sight_L_Init)
  {
    for(int y=80;y>=Sight_Init;y--)      
    {  //�����������㶪ʧ���Ҷ��Ǻ�ɫ�����ж���������
      // if(Img[y][Left[y]] == ImgBlack && Img[y-1][Left[y-1]] == ImgBlack && Img[y-2][Left[y-2]] == ImgBlack)
      if(Lost_L[y] == LostBlack && Lost_L[y-1] == LostBlack && Lost_L[y-2] == LostBlack)
      {
        Sight_L=y+1;
        break;
      }
      //��߽��߱ȽϿ���
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
    {   //�����������㶪ʧ���Ҷ��Ǻ�ɫ�����ж���������
      //   if(Img[y][Right[y]] == ImgBlack && Img[y-1][Right[y-1]] == ImgBlack && Img[y-2][Right[y-2]] == ImgBlack)
      if(Lost_R[y] == LostBlack && Lost_R[y-1] == LostBlack && Lost_R[y-2] == LostBlack)
      { 
        Sight_R=y+1;
        break;    
      }
      //      �ұ߽��߱ȽϿ���
      if(y<80 && Right[y]<10 && Right[y+1]<20 && Right[y]<Right[y+1] && Sight_R==Sight_R_Init)   
      {
        Sight_R=y;
        break;
      }    
    }
  }
//  if(CarType == Later)
//  {
//    //ȥ���˵�Ӱ�죬4.24.16.2448
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
  if(Sight_L>Sight_R)   // ǰհȡ��Զ��
    Sight=Sight_R;
  else
    Sight=Sight_L;
  if(RoadType == Overtake_Left_ing)
    Sight = Sight_L;
  if(RoadType == Overtake_Right_ing)
    Sight = Sight_R;  
}
/****************************
���ã��ж���������
****************************/
void judge_road()
{
  uint8 judge=0;        //�������������Ŀ����Ҫ���жϳ�һ���������ͺ��ٽ��������������͵��жϣ���������
 // judge = OvertakeAreaFind(judge);    //�ӿ�����ʶ��
  //judge = OvertakeArea_ing(judge);    //���볬����
  //  Overtake_LaterToFront();//�󳵱�ǰ��
  //  Overtake_FrontReturn(); //ǰ��������������
  //  Overtake_LaterReturn();               //�󳵴ӳ���������
  //  OvertakeArea_Over();                  //��������
    //Overtake_Middle();
  //  Overtake_Return_GenZong();
  
  //�ͳ�������ͻ��Ҫ�ģ���
  CrossCheck();                      //��ʮ�ּ��
  judge = judge_crossxieL(judge);    //��б��ʮ��          
  judge = judge_crossxieR(judge);    //��б��ʮ��
  
  StraightCheck();                  //ֱ�߼��
  
  GoInBendCheck();                  //������
  //OutsideCheck();                   //������
  
}
/****************************
���ã���б��ʮ���жϼ�����
�������Ƿ���Ҫ�ж�
���أ��жϽ��
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
      //�ұ���������ɫ��ͬʱ�������������ʧ
      if(Lost_L[y]==0 && Lost_L[y+1]==0 && Lost_R[y]==LostWhite && Lost_R[y+1]==LostWhite)
        a++;
      else
        a=0;
      //��߽翪ʼ�����ߣ��յ�֮��
      if(a>5 && Left[y]>Left[y-1] && Left[y-1]>Left[y-2] && Left[y-2]>Left[y-3] && Left[y-3]>Left[y-4] && Left[y-4]>Left[y-5])
      {
        Cross_Xielv=jisuan_xielv_orginal(Left,y,y-10);
        Cross_Xielv2=jisuan_xielv_orginal(Left,y+10,y+1);
        if(Cross_Xielv>0.05  && Cross_Xielv2<0 && Cross_Xielv2>-5 )//б�����䣬������б��ʮ�֣������������
        { 
          //          uint8 n=159-Left[y];
          //          uint8 WhitePoint=0,i;
          //          uint8 x=Left[y];
          //          for(i=0;i<n;i++)            //ͳ���ҷ��׵���Ŀ������ȫ������    
          //          {
          //            if(Img[y][x+i]==ImgWhite) //����֪������ٻ�Ϊ100�Ⱦ�ͷ���ҷ��Ƿ�Ϊȫ��
          //              WhitePoint++;
          //          }
          //������
          if(Lost_R[y-1]==LostWhite && Lost_R[y-2]==LostWhite)
          {                                               //�������������
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
        int left=(j-y-10)*Cross_Xielv2+Left[y+10];               //����б�ʲ���
        if(left>Right_Init)
          left = Right_Init;
        Left[j] = left;
        Middle[j]=fanjiaozheng_x(j,Left[j],ROAD_HALF);  //��У������
        Right[j]=fanjiaozheng_x(j,Left[j],ROAD_W);
        if(Middle[j]>Right_Init)                        //��ֹ�����ˣ�Խ����
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
���ã���б��ʮ���жϼ�����
�������Ƿ���Ҫ�ж�
���أ��жϽ��
****************************/
int judge_crossxieL(uint8 judge)
{
  uint8 volatile flag_guai=0,y=0,a=0;
  int Distance_YuZhi = 400;//��ʮ��3���ڱ���б��ʮ���ж�
  //if(judge==0 && (RoadType == 0 || RoadType == Cross) && (Distance_Cross != 0 && Distance-Distance_Cross < Distance_YuZhi))
  if(judge==0 && (RoadType == 0 || RoadType == Cross))
  {
    float xielv,xielv2;
    for(Row=108;Row>=Sight;Row--)
    {
      //���������ʧ��ͬʱ�ұ���������ʧ
      if(Lost_R[Row]==0  && Lost_R[Row+1]==0 && Lost_L[Row]==LostWhite && Lost_L[Row+1]==LostWhite)
        a++;
      else
        a=0;
      //�ұ߽翪ʼ�����ߣ��յ�֮��
      if(a>5 && Right[Row]<Right[Row-1]&& Right[Row-1]<Right[Row-2] && Right[Row-2]<Right[Row-3] && Right[Row-3]<Right[Row-4]&& Right[Row-4]<Right[Row-5])
      {
        //����ֵԽ��Խ��б
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
          //            if(Img[Row][x-i]==ImgWhite)   //ͳ���ҷ��׵���Ŀ������ȫ������
          //              WhitePoint++;             //����֪������ٻ�Ϊ100�Ⱦ�ͷ���ҷ��Ƿ�Ϊȫ��
          //          } 
          //������
          if(Lost_L[Row-1]==1 && Lost_L[Row-2]==1)
          {                                                //�������������
            flag_guai=1;
            break;
          }
        }
        
      }
    }  
    if(flag_guai==1 )//�ҵ��յ�
    {
      flag_guai=0;
      int j;
      int temp_x,temp_y;
      RoadType=CrossL;
      float xielv3=10/(Right[Row+10]-Right[Row]);
      for(j=Row+9;j>Sight_Init;j--)
      {
        int right=(j-Row-10)*Cross_Xielv2+Right[Row+10];       //��б�ʲ���
        if(right<Left_Init)
          right=Left_Init;
        Right[j] = right;
        Middle[j]=fanjiaozheng_x(j,Right[j],-ROAD_HALF);    //��У������
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
���ã���������ƫ��
���أ�ͼ��ƫ��
****************************/
float jisuan_piancha()
{
  int y=0;
  int Sight_Temp;
  float piancha_avr=0;
  float piancha=0;
  ValidLine=0;         //��Ч������
  if(Sight>Sight_Init_Turn)
    Sight_Temp = Sight;
  else
    Sight_Temp = Sight_Init_Turn;
  for(y=119;y>=Sight_Temp;y--)  //�����߽���Զ�õ�1��5��������ƫ����Զ�õ�1��2
  {
    //  if(SelectRowFlag[y]==1)      //ֻ�����ض��е�ƫ��
    {
      // if((Lost_L[y]==LostWhite && Lost_R[y]==LostWhite) || (Lost_L[y]==LostBlack && Lost_R[y]==LostBlack)||(Middle[y]==Left_Init) || (Middle[y]==Right_Init))
      if((Lost_L[y]==LostWhite && Lost_R[y]==LostWhite) || (Lost_L[y]==LostBlack && Lost_R[y]==LostBlack))
        
      {//���в�����ƫ�����
      }      
      else
      {
        //piancha+=((int)(Middle[y]-80)*Img_P[y/10]);
        piancha+=((int)(Middle[y]-69)*Img_P[y/10]);
        ValidLine++;              //�ۼ���Ч����
      }
    }
  }
  piancha_avr=(piancha)/ValidLine;    //ƽ��ƫ�����Ҫ�ĸ���Ȩ�ر�
  
  if(piancha_avr>54)
    piancha_avr=54;
  if(piancha_avr<-54)
    piancha_avr=-54;
  
//  if(piancha_avr>65)
//    piancha_avr=65;
//  if(piancha_avr<-65)
//    piancha_avr=-65;
  return piancha_avr;//+10; //�������ͷ��
}
/****************************
���ã������߼��
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
δ��֤
���ã�ֱ�������ж�
****************************/
void GoInBendCheck()
{
  uint8 YuZhi = 1;     
  uint8 Error_YuZhi = 40;
  uint8 Sight_YuZhi = 34;
  if(StraightNum>YuZhi)    //֮ǰ��������ͼ����ֱ��
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
δ��֤
���ã������ж�
****************************/
void GoOutBendCheck()
{
  if(BendNum>3)
  {
    if(Sight<40 && (ImgNum-Img_GoOutBend)>10)//��һ���ж���Ϊ�˱�֤�������ֻ����һ�Σ�����ֻһ����ĺ��𡣡���
    {
      Flag_GoOutBend = 1;
      Img_GoOutBend = ImgNum;
      RoadType = GoOutBend;
    }
  }
}
/****************************
δ��֤
���ã�������ж�
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
δ��֤
���ã�ֱ���ж�
****************************/
void StraightCheck()
{
  {   
    uint8 Yuzhi=10;
    if(AbsMiddleError<Yuzhi && Sight - Sight_Init<2)//ƫ��С��ǰհԶ
    {
      //      float XieLv_YuZhi = 0.2;
      //      float XieLv_Top = (float)((int)Middle[Sight_Init_Turn]-(int)Middle[(Sight_Init_Turn+119)/2])/(float)((Sight_Init_Turn+119)/2-Sight_Init_Turn);
      //      float XieLv_Bottom = (float)((int)Middle[(Sight_Init_Turn+119)/2]-(int)Middle[119])/(float)(119-(Sight_Init_Turn+119)/2);
      //      if(XieLv_Top>=0 && XieLv_Bottom>=0 && Myabsfloat(XieLv_Top-XieLv_Bottom)<XieLv_YuZhi)
      {
        CurveType = Straight;
        if(Count_Total-Count_Total_Last>335)//ÿ5CM����һ��
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
      if(StraightNum>0)                //�������������ж�Ϊֱ�����Ž��������͸�ֵΪֱ��
    {                                //�Ͼ��ж�Ϊֱ���󣬵���ٶȻ�˲�ӣ�������������
     //  CurveType = Straight;
    }
  }
}
//��ʮ�ּ�⣬ʮ�����⴦�����ö�ֵP����
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
    if(Num >29)//�ײ����߶����ߣ��Һܶ��ж�������
    {
      // led(LED0,LED_ON);
      // if(abs_sub(Middle[Sight],Middle[119])>15)//ʮ�ֺ����������ֱ��
      {
        //   flag = 1;
        RoadType = Cross;
        Distance_Cross = Distance;//��¼·����Ҫ��Ϊ�˽���ʮ�ֺ���ʮ������
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
//�������͹�������
void CurveType_Roll()
{
  //  int YuZhi = 10;               //ÿ�߹�һ����·�̸���һ�Ρ�
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
//�ӿ�����ʶ��
uint8 OvertakeAreaFind(uint8 judge)
{
  if(CarType == Front && judge == 0 && RoadType == 0)
  {
    Road_OvetakeFind_X    = 0;
    int YuZhi_Road = 12;
    float YuZhi_XieLv = 0.2;
    int YuZhi_WhitePoint = 52;
    Y_Overtake_Start = Sight;
    // ���ıȽ�Զ             
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
//���볬����
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
      for(uint8 y =116;y>Sight_L+3;y--)//û��ͻȻ���ҵĵ㣬�龰�����볬����ǰ����Ͽ����ұߵײ��ܶ��Ҳ��������ܻ�����Ϊ���볬����
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
      for(uint8 y =116;y>Sight_R+3;y--)//û��ͻȻ���ҵĵ㣬�龰�����볬����ǰ����Ͽ����ұߵײ��ܶ��Ҳ��������ܻ�����Ϊ���볬����
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
//�ӳ���������
//uint8 OvertakeArea_Return(uint8 judge)
//{
//  if(judge == 0 && CarType == Front && RoadType == Overtake_Left_ing)
//  {
//    if(Sight_L>45)              //���ϵ�������˴����￪ʼ����
//      RoadType = Overtake_Left_Return;
//  }
//  if(judge == 0 && RoadType == Overtake_Right_ing)
//  {
//    if(Sight_L>45)              //���ϵ�������˴����￪ʼ����
//      RoadType = Overtake_Right_Return;
//  }
//  return 0;
//}
//��������
void OvertakeArea_Over()
{
  if(RoadType == Overtake_Right_Rerturn_Execute)
  {
    if(Lost_R[116] == 0 && Lost_R[115] == 0)
      RoadType = Normal;
  }
}
//�������󳵱�ǰ��
void Overtake_LaterToFront()
{//�ҳ�����
  if(CarType == Later && RoadType_AnotherCar == Overtake_Right_ing)
  {
    int YuZhi = 110;
    memset(Lost_R,LostWhite,CAMERA_H);
    for(uint8 y = 119;y>75;y-=2)
    {
      for(uint8 x = 80;x<Right_Init;x++)
      {
        if(Img[y][x]==ImgBlack)//�ҵ��ڵ�
        {//��һ��ȷ��
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
      Distance_Overtake_Right_GoStraight = Distance;//��¼·�̣��ٹ�һ�ξ����ٱ�Ϊ���м�ģʽ
  }
  
  //�󳬳���
  if(CarType == Later && RoadType_AnotherCar == Overtake_Left_ing)
  {
    int YuZhi = 110;
    memset(Lost_L,LostWhite,CAMERA_H);
    for(uint8 y = 119;y>75;y-=2)
    {
      for(uint8 x = 80;x>Left_Init;x--)
      {
        if(Img[y][x]==ImgBlack)//�ҵ��ڵ�
        {//��һ��ȷ��
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
      Distance_Overtake_Left_GoStraight = Distance;//��¼·�̣��ٹ�һ�ξ����ٱ�Ϊ���м�ģʽ
    return ;
  }
}
//ǰ��������������
void Overtake_FrontReturn()
{
  //
  if(CarType == Front && RoadType == Overtake_Right_GoStraight)
  {
    //�ұߵײ��ܿ���
    if(Lost_R[CAMERA_H-4] == 0)
      RoadType = Normal;
  }
  if(CarType == Front && RoadType == Overtake_Left_GoStraight)
  {
    //�ұߵײ��ܿ���
    if(Lost_L[CAMERA_H-4] == 0)
      RoadType = Normal;
  }
}
void Overtake_LaterReturn()
{
  //�ҳ�����
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
  
  //�󳬳���
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
    //        Lost_L[y] = Lost_R[y] = LostWhite;        //Ϊ�˼���ƫ��ʱ��ʹ����Щ��
    //      for(uint8 y = Sight_L;y>Sight_Init;y--)
    //      {
    //        for(uint8 x = 157;x>60;x--)
    //        {
    //          if(Img[y][x]==ImgBlack)//�ҵ��ڵ�
    //          {//��һ��ȷ��
    //            if(Img[y][x-1]==ImgBlack && Img[y][x-2]==ImgBlack && Img[y][x+1]==ImgWhite && Img[y][x+2]==ImgWhite)
    //            { 
    //              Left[y]=x+2;
    //              Lost_L[y]=0;        //���߱�־λ����
    //              Middle[y] = fanjiaozheng_x(y,Left[y],25); 
    //              break;        //�˳�ѭ�� 
    //            }
    //          }
    //        }
    //        
    //      }
    //    }
    //    if(RoadType == Overtake_Right_Return)
    //    {
    //      for(uint8 y = 119;y>Sight_R;y--)
    //        Lost_L[y] = Lost_R[y] = LostWhite;        //Ϊ�˼���ƫ��ʱ��ʹ����Щ��
    //      for(uint8 y = Sight_R;y>Sight_Init;y--)
    //      {
    //        for(uint8 x = 2;x<100;x++)
    //        {
    //          if(Img[y][x]==ImgBlack)//�ҵ��ڵ�
    //          {//��һ��ȷ��
    //            if(Img[y][x+1]==ImgBlack && Img[y][x+2]==ImgBlack && Img[y][x-1]==ImgWhite && Img[y][x-2]==ImgWhite)
    //            { 
    //              Right[y]=x-2;
    //              Lost_R[y]=0;        //���߱�־λ����
    //              Middle[y] = fanjiaozheng_x(y,Right[y],-25); 
    //              break;        //�˳�ѭ�� 
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
//������һ���ұ߽磬��Ҫ��Ϊǰ����������Ե����һ��
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
    Y_Stop = Sight;//�Ѿ����������������Ӧ�ñ��ҵ�����Ӧ���ҷ�չ
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
    //�ұ߽���������� 
    for(x=Start_R;x<Final_R;x++) 
    {
      if(Img[Row][x]==ImgBlack)//�ҵ��ڵ�
      {//��һ��ȷ��
        if(Img[Row][x+1]==ImgBlack && Img[Row][x+2]==ImgBlack && Img[Row][x-1]==ImgWhite && Img[Row][x-2]==ImgWhite && Img[Row][x-3]==ImgWhite)
        { 
          if(Row<=Light_Bottom && Row>=Light_Top)
          {
            if(x-2<Light_Right)
            {}
            else
            {
              Right[Row]=x-2;
              Lost_R[Row]=0;        //���߱�־λ����
              break;        //�˳�ѭ�� 
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
          break;//�˳�ѭ��
        }
      }           
    }
    Last_R=Right[Row];     //������һ�ε�ֵ
    if(Flag_NotRefind == 0 && Right[Row]==Right_Init)//˵���ұ߽�û����ȡ��
    {
      Last_R=Right[Row]=Refind_R(Row);    
    }
    Right_Correct(Row);
  }
}
//������һ����߽磬��Ҫ��Ϊǰ����������Ե����һ��
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
    Y_Stop_GenZongAgain = Sight;//�Ѿ����������������Ӧ�ñ��ҵ�����Ӧ���ҷ�չ
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
    //��߽������ ���� 
    if(Last_L-Window>Left_Init)
      Final_L=Last_L-Window;       
    if(Last_L+Window<Right_Init)
      Start_L=Last_L+Window;
    else
      Start_L=Right_Init;
    for(x=Start_L;x>Final_L;x--)     
    {
      if(Img[Row][x]==ImgBlack)//�ҵ��ڵ�
      {//��һ��ȷ��
        if(Img[Row][x-1]==ImgBlack && Img[Row][x-2]==ImgBlack && Img[Row][x+1]==ImgWhite && Img[Row][x+2]==ImgWhite  && Img[Row][x+3]==ImgWhite)
        {
          if(Row<=Light_Bottom && Row>=Light_Top)
          {
            if(x+2>Light_Left)
            {}
            else
            {
              Left[Row]=x+2;
              Lost_L[Row]=0;        //���߱�־λ����
              break;        //�˳�ѭ�� 
            }                  
          }
          else
          {
            Left[Row]=x+2;
            Lost_L[Row]=0;        //���߱�־λ����
          }
          if(Row>Y_Stop_GenZongAgain && Left[Row]>Left[Row+1])
          {
            Left[Row] = Left[Row+1];
            Lost_L[Row] = LostWhite;
          }
          break;        //�˳�ѭ�� 
        }
      }           
    }
    Last_L=Left[Row];     //������һ�ε�ֵ
    if(Flag_NotRefind == 0 && Left[Row]==Left_Init)//˵��û����ȡ���߽�
    {
      Last_L=Left[Row]=Refind_L(Row);     //�ٴ�Ѱ��
    }
    Left_Correct(Row);
  }
}
//�󳵾����������߽�
void Left_Correct(uint8 y)
{
  
  int Change_YuZhi = 13;
  // if()
  //�׵��Ϸ�
  if((y<Light_Bottom) && (Left[y]>(Light_Left-10)))
  {
    //  if(abs_sub(Left[y],Left[y+1])>Change_YuZhi)
    if((abs_sub(Right[y],Light_X)>abs_sub(Light_X,Left[y])) && (abs_sub(Left[y+1],Left[y])>Change_YuZhi) && Lost_L[y] == 0)
    {
      //      if(y<80 && Lost_R[y+25] == LostWhite && Lost_R[y+28] == LostWhite && Lost_L[y+5] == LostWhite  && Lost_L[y+10] == LostWhite  && Lost_L[y+15] == LostWhite  && Lost_L[y+20] == LostWhite)
      //      {}//б��ʮ��
      //      else
      {
        int Start = Left[y]-4;
//        if(Left[y+1]<10)
//          Last_L = Left[y] = Left[y+1];//4.19.12.1211
//        else
          Last_L = Left[y] = Left_Init;//�Ȼָ�Ĭ��ֵ�����ҵ����Ի����
        if(Img[y][Left_Init] == ImgWhite)
          Lost_L[y] = LostWhite;
        if(Img[y][Left_Init] == ImgBlack)
          Lost_L[y] = LostBlack;        for(int x = Start;x>Left_Init;x--)
          {
            if(Img[y][x]==ImgBlack)//�ҵ��ڵ�
            {//��һ��ȷ��
              if(Img[y][x-3]==ImgBlack && Img[y][x-2]==ImgBlack && Img[y][x-1]==ImgBlack
                 && Img[y][x+3]==ImgWhite && Img[y][x+2]==ImgWhite && Img[y][x+1]==ImgWhite)
              { 
                if(abs_sub(x,Left[y+1])<=Change_YuZhi)
                { 
                  Last_L = Left[y]=x+2;
                  Lost_L[y]=0;        //���߱�־λ����
                  break;        //�˳�ѭ�� 
                }
              }
            }  
          } 
      }
      ErrorType = LeftCorrect_1; 
    }
  }
  //�׵��Ϸ�2
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
        Last_L = Left[y] = Left_Init;//�Ȼָ�Ĭ��ֵ�����ҵ����Ի����
        if(Img[y][Left_Init] == ImgWhite)
          Lost_L[y] = LostWhite;
        if(Img[y][Left_Init] == ImgBlack)
          Lost_L[y] = LostBlack;
        for(int x = Start;x>Left_Init;x--)
        {
          if(Img[y][x]==ImgBlack)//�ҵ��ڵ�
          {//��һ��ȷ��
            if(Img[y][x-3]==ImgBlack && Img[y][x-2]==ImgBlack && Img[y][x-1]==ImgBlack
               && Img[y][x+3]==ImgWhite && Img[y][x+2]==ImgWhite && Img[y][x+1]==ImgWhite)
            { 
              if(abs_sub(x,Left[y+1])<=Change_YuZhi)
              { 
                Last_L = Left[y]=x+2;
                Lost_L[y]=0;        //���߱�־λ����
                break;        //�˳�ѭ�� 
              }
            }
          }  
        }  
        ErrorType = LeftCorrect_2; 
      }
    }
  }
  //�׿��ϵĵ㱻�����߽�
  if((y>Light_Top-2) && y<Light_Bottom+2 && (abs_sub(Right[y],Light_X)>abs_sub(Light_X,Left[y])) && (abs_sub(Left[y],Left[y+1])<=Change_YuZhi) && Lost_L[y] == 0 && Left[y]>=Light_Left-3)
  {
    int Start = Left[y]-4;
    Last_L = Left[y] = Left_Init;//�Ȼָ�Ĭ��ֵ�����ҵ����Ի����
    if(Img[y][Left_Init] == ImgWhite)
      Lost_L[y] = LostWhite;
    if(Img[y][Left_Init] == ImgBlack)
      Lost_L[y] = LostBlack;
    for(int x = Start;x>Left_Init;x--)
    {
      if(Img[y][x]==ImgBlack)//�ҵ��ڵ�
      {//��һ��ȷ��
        if(Img[y][x-3]==ImgBlack && Img[y][x-2]==ImgBlack && Img[y][x-1]==ImgBlack
           && Img[y][x+3]==ImgWhite && Img[y][x+2]==ImgWhite && Img[y][x+1]==ImgWhite)
        { 
          if(x<Light_Left-4)
          {
            Last_L = Left[y]=x+2;
            Lost_L[y]=0;        //���߱�־λ����
            break;        //�˳�ѭ�� 
          }
        }
      }  
    }
    ErrorType = LeftCorrect_3;
  }
  //ǰ�������ڲ�����Ϊ�߽�
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
  //U����ĩ�ˣ��Ҳ�����Դ��ֻ����ǰ������
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
        //y+1��Ҳ���LostWhite������ʱ�������һ�����ҵ������ҵ��ǳ�
        ErrorType = LeftCorrect_5;
      }
    }          
  }
  //���ĩ�ˣ��׵��Ϸ��Ҵ�
  if(Light_X<34)
  {
    if(Left[y]>Left[y+1])
    {
      Last_L = Left[y] = Left[y+1];
      Lost_L[y+1] = Lost_L[y] = LostWhite;
      //y+1��Ҳ���LostWhite������ʱ�������һ�����ҵ������ҵ��ǳ�
      ErrorType = LeftCorrect_6;
    }
  }
}
//�󳵾���������ұ߽�
void Right_Correct(uint8 y)
{
  int Change_YuZhi = 14;
  //�׵��Ϸ�
  //  if(0)
  if(y<Light_Bottom && (Right[y]<(Light_Right+10)) )
  {  
    if((abs_sub(Right[y],Light_X)<abs_sub(Light_X,Left[y])) && (abs_sub(Right[y],Right[y+1])>Change_YuZhi) && Lost_R[y] == 0)
    {
      //      if(y<80 && Lost_L[y+25] == LostWhite && Lost_L[y+28] == LostWhite && Lost_R[y+5] == LostWhite  && Lost_R[y+10] == LostWhite  && Lost_R[y+15] == LostWhite  && Lost_R[y+20] == LostWhite)
      //      {}//б��ʮ��
      //      else
      {
        int Start = Right[y]+4;
//        if(Right[y+1]>150)
//          Last_R = Right[y] = 150;//4.19.12.1211
//        else
          Last_R = Right[y] = Right_Init;//�Ȼָ�Ĭ��ֵ�����ҵ����Ի����
        if(Img[y][Right_Init] == ImgWhite)
          Lost_R[y] = LostWhite;
        if(Img[y][Right_Init] == ImgBlack)
          Lost_R[y] = LostBlack;
        for(int x = Start;x<Right_Init;x++)
        {
          if(Img[y][x]==ImgBlack)//�ҵ��ڵ�
          {//��һ��ȷ��
            if(Img[y][x+3]==ImgBlack && Img[y][x+2]==ImgBlack && Img[y][x+1]==ImgBlack
               && Img[y][x-4]==ImgWhite  && Img[y][x-3]==ImgWhite && Img[y][x-2]==ImgWhite && Img[y][x-1]==ImgWhite)
            { 
              //  Road_X = real_x[y][x] - real_x[y][Left[y+1]];
              if(abs_sub(x,Right[y+1])<Change_YuZhi)
              { 
                Last_R = Right[y]=x-2;
                Lost_R[y]=0;        //���߱�־λ����
                break;        //�˳�ѭ�� 
              }
            }
          }
        }  
      }  
      ErrorType = RightCorrect_1;
    }
  }
  //�׵��Ϸ�2
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
        Last_R = Right[y] = Right_Init;//�Ȼָ�Ĭ��ֵ�����ҵ����Ի����
        if(Img[y][Right_Init] == ImgWhite)
          Lost_R[y] = LostWhite;
        if(Img[y][Right_Init] == ImgBlack)
          Lost_R[y] = LostBlack;
        for(int x = Start;x<Right_Init;x++)
        {
          if(Img[y][x]==ImgBlack)//�ҵ��ڵ�
          {//��һ��ȷ��
            if(Img[y][x+3]==ImgBlack && Img[y][x+2]==ImgBlack && Img[y][x+1]==ImgBlack
               && Img[y][x-4]==ImgWhite  && Img[y][x-3]==ImgWhite && Img[y][x-2]==ImgWhite && Img[y][x-1]==ImgWhite)
            { 
              //  Road_X = real_x[y][x] - real_x[y][Left[y+1]];
              if(abs_sub(x,Right[y+1])<Change_YuZhi)
              { 
                Last_R = Right[y]=x-2;
                Lost_R[y]=0;        //���߱�־λ����
                break;        //�˳�ѭ�� 
              }
            }
          }  
        }  
        ErrorType = RightCorrect_2;
      }
    }
  }
  //�׿��ϵĵ�����Ϊ�߽�
  // if(0)
  if((y>(Light_Top-2)) && y<Light_Bottom+2 && (abs_sub(Right[y],Light_X)<abs_sub(Light_X,Left[y])) && (abs_sub(Right[y],Right[y+1])<=Change_YuZhi) && Lost_R[y] == 0 && Right[y]<=Light_Right+3)
  {
    int Start = Right[y]+4;
    Last_R = Right[y] = Right_Init;//�Ȼָ�Ĭ��ֵ�����ҵ����Ի����
    if(Img[y][Right_Init] == ImgWhite)
      Lost_R[y] = LostWhite;
    if(Img[y][Right_Init] == ImgBlack)
      Lost_R[y] = LostBlack;
    for(int x = Start;x<Right_Init;x++)
    {
      if(Img[y][x]==ImgBlack)//�ҵ��ڵ�
      {//��һ��ȷ��
        if(Img[y][x+3]==ImgBlack && Img[y][x+2]==ImgBlack && Img[y][x+1]==ImgBlack
           && Img[y][x-4]==ImgWhite  && Img[y][x-3]==ImgWhite && Img[y][x-2]==ImgWhite && Img[y][x-1]==ImgWhite)
        { 
          if(x>Light_Right+4)
          {
            Last_R = Right[y]=x-2;
            Lost_R[y]=0;        //���߱�־λ����
            break;        //�˳�ѭ�� 
          }
        }
      }  
    } 
    ErrorType = RightCorrect_3;        
  }  
  
  //ǰ�������ڲ�����Ϊ�߽�
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
  //U����ĩ�ˣ��Ҳ�����Դ��ֻ����ǰ������
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
        //y+1��Ҳ���LostWhite������ʱ�������һ�����ҵ������ҵ��ǳ�
        ErrorType = RightCorrect_5;
      }
    }  
  }
  //���ĩ�ˣ��׵��Ϸ��Ҵ�
  //if(0)
  if(Light_X>126)
  {
    if(Right[y]<Right[y+1])
    {
      Last_R = Right[y] = Right[y+1];
      Lost_R[y+1] = Lost_R[y] = LostWhite;
      //y+1��Ҳ���LostWhite������ʱ�������һ�����ҵ������ҵ��ǳ�
      ErrorType = RightCorrect_6;
    }
  }
}
//���ǳ��������߼���
void Middle_Triangle()
{
  if(CarType == Later)
  {
    if(RoadType == Triangle_Left_GoIn)
    {
      uint8 Y_Start = 116;
      uint8 Y_Stop  = Sight;
      int middle    = 80;
      //����Ѱ�Ҳ�����ʼ��
      for(uint8 y = 116;y>Sight;y--)
      {
        if(Lost_L[y] == LostWhite && Lost_R[y] == LostWhite)
        {
          Y_Start = y;
          break;
        }
      }
      //Ѱ�Ҳ��߽�����
      for(uint8 y = Y_Start;y>Sight;y--)
      {
        if(Lost_L[y] == 0 || Lost_R[y] == 0)
        {
          Sight_L = Sight_R = Sight = y;
          Y_Stop = y;
          break;
        }
      }
      //ȷ��������ʼ��
      for(uint8 y = Y_Start;y<116;y++)
      {
        if(Lost_L[y] == 0 && Lost_R[y] == 0)
        {
          Y_Start = y;
          break;
        }
      }
      //��ʼ����
      for(uint8 y = Y_Start;y>Y_Stop;y--)
      {
        middle = Middle[y+1]-(y-Y_Start)*(y-Y_Start)*ImgCurveDegree;
        if(middle<Left_Init)
          middle = Left_Init;
        Middle[y] = middle;
        Left[y] = Left_Init;
        Right[y] = Right_Init;
        Lost_L[y] = Lost_R[y] = 0;//Ϊ���ü���ƫ��ʱ���ñ���
      }   
    }
    if(RoadType == Triangle_Right_GoIn)
    {
      uint8 Y_Start = 116;
      uint8 Y_Stop  = Sight;
      int middle    = 80;
      //����Ѱ�Ҳ�����ʼ��
      for(uint8 y = 116;y>Sight;y--)
      {
        if(Lost_L[y] == LostWhite && Lost_R[y] == LostWhite)
        {
          Y_Start = y;
          break;
        }
      }
      //Ѱ�Ҳ��߽�����
      for(uint8 y = Y_Start;y>Sight;y--)
      {
        if(Lost_L[y] == 0 || Lost_R[y] == 0)
        {
          Sight_L = Sight_R = Sight = y;
          Y_Stop = y;
          break;
        }
      }
      //ȷ��������ʼ��
      for(uint8 y = Y_Start;y<116;y++)
      {
        if(Lost_L[y] == 0 && Lost_R[y] == 0)
        {
          Y_Start = y;
          break;
        }
      }
      //��ʼ����
      for(uint8 y = Y_Start;y>Y_Stop;y--)
      {
        middle = Middle[y+1]+(y-Y_Start)*(y-Y_Start)*ImgCurveDegree;
        if(middle>Right_Init)
          middle = Right_Init;
        Middle[y] = middle;
        Left[y] = Left_Init;
        Right[y] = Right_Init;
        Lost_L[y] = Lost_R[y] = 0;//Ϊ���ü���ƫ��ʱ���ñ���
      }   
    }
  }
}
//������
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
    // ���ǰ��LCD
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