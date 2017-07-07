/*
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2016,���к�
 *     All rights reserved.
 *
 * @file       Arithmetic.c
 * @brief      ǰ���жϺ�����
 * @author     С˼ͬѧ ���к�
 * @version    v1.3 ���ӷ���2
 * @date       2016.2.27
***************************************************
 * ��ֲ˵����
 *  0.�� Arithmetic.h ͷ�ļ�����
 *  1.��main.c ���������  extern RoadScanType RoadScan;
 *  2.�ڳ�ʼ����ʱ�����   RoadScan_Init(); 
 *  3.�ڵ��õ�ʱ��ʹ��     RoadScan.Plan.Plan1();
      ɨ�������Զ�װ��RoadScan�ṹ����
 *  4.RoadScan.Res�д�����״̬�ͽ��
 */

#include "common.h"
#include "include.h"
#include "Arithmetic.h"

extern uint8 Img[CAMERA_H][CAMERA_W];           //��ѹ�����ͼ��
RoadScanType RoadScan;                          //����ɨ��ṹ���ʼ��

uint8 RSP_vCL_MAX = RSP_CLIMIT_MAX;
uint8 RSP_vCL_MIN = RSP_CLIMIT_MIN;
uint8 RSP_vWL_MAX = RSP_WLIMIT_MAX;
uint8 RSP_vWL_MIN = RSP_WLIMIT_MIN;
uint8 RSP_vHL_MAX = RSP_HLIMIT_MAX;
uint8 RSP_vHL_MIN = RSP_HLIMIT_MIN;

void LCD_RecBlank(uint8 x, uint8 y, uint8 Wide, uint8 High, uint16 Color, uint8_t wLine)
{
  Site_t site;
  Size_t size;

  site.x = x - (Wide >> 1);
  site.y = y - (High >> 1);
  size.H = wLine;
  size.W = Wide;
  LCD_rectangle(site, size, Color);
  
  size.H = High;
  size.W = wLine;
  LCD_rectangle(site, size, Color);
  
  site.x = x + (Wide >> 1);
  site.y = y - (High >> 1);
  LCD_rectangle(site, size, Color);
  
  site.x = x - (Wide >> 1);
  site.y = y + (High >> 1);
  size.H = wLine;
  size.W = Wide;
  LCD_rectangle(site, size, Color);
  
}

/*!
 *  @brief      ����ɨ���ʼ������ ver1.0
 *  @note       ����ɨ��ṹ�壬���з������򡢹����Լ�����ĳ�ʼ����
 *  @date       2016.2.10
 */
void RoadScan_Init(void)
{
  RoadScan.Plan       = RoadScan_Plan2;
  RoadScan.PlanTri    = RoadScan_Triangle;
  RoadScan.PlanStart  = RoadScan_StartLine;
  RoadScan.Res.state  = null;
  RoadScan.tRes.state = null;
  RoadScan.sRes.state = null;
}

/*
 *  @brief      ����ɨ�� ����һ ver1.0
 *  @note       Ѱ�ҷ�գ��ҷ��ͼ�η���һ����������򲢶�λ
 *  @date       2016.2.27
 */
void RoadScan_Plan2(void)
{
  static uint8_t BreakOffsetX = 0, BreakOffsetY = 0;
  static int8_t OffsetW = 0, OffsetH = 0;
  static RoadResType Res;
  static RoadResType ResKeep;
  uint8 hIndex = 0, wIndex = 0;                 //�߶����������������ʼ��
  RoadSectionCellType Short = {0,0,none};       //����������Ԫ��ʼ��
  
//  if(RoadScan.Res.state == Exist)
//  {
//    OffsetW = RoadScan.Res.Wide - Res.Wide;
//    OffsetH = RoadScan.Res.High - Res.High;
//    RSP_vHL_MAX += OffsetW;
//    RSP_vWL_MAX += OffsetH;
//    RSP_vHL_MIN  = RSP_vHL_MAX / 5;
//    RSP_vWL_MIN  = RSP_vWL_MAX / 5;
//    LCD_RecBlank(RoadScan.Res.x + 42, RoadScan.Res.y + 100, RSP_vWL_MAX, RSP_vHL_MAX, RED,1);
//    LCD_RecBlank(RoadScan.Res.x + 42, RoadScan.Res.y + 100, RSP_vWL_MIN, RSP_vHL_MIN, BLUE,1);
//  } 
//  else
//  {
//    
//  }
//  vLCD_Printf(0, 240, WHITE, BLACK, "W1:%3d,H1:%3d", RSP_vWL_MAX, RSP_vHL_MAX);
//  vLCD_Printf(0, 256, WHITE, BLACK, "W2:%3d,H2:%3d", RSP_vWL_MIN, RSP_vHL_MIN);
  
  
  //�����Ϸ���ʼɨ�裬Ѱ�Ұ���
  if(RoadScan.Res.state == Exist) Res = RoadScan.Res;
  if(Res.state == Exist)
  {
    RoadScan_RegionJudgePart(&RoadScan.Res, Res.x, Res.y);
    if(RoadScan.Res.state == Exist)          //���ڣ�����β�ˣ��˳�˫for
    {      
      if(abs(Res.x - RoadScan.Res.x) > 50 ||abs(Res.y - RoadScan.Res.y) > 50)
      {
          RoadScan.Res = Res;
      }
      return;
    }
  }
  for(hIndex = RangeEndHigh; hIndex > RangeStartHigh; hIndex--)
  {
    Short.Right = 0;                            //���鵥Ԫ�ṹ���ʼ����ÿ����Ҫ
    Short.Left  = 0;
    for(wIndex = 0; wIndex < RSP_WIDE; wIndex++)
    {
      if(Img[hIndex][wIndex] == RSP_WHITE)           //Ѱ�ҵ���ɫ
      {
        if(Img[hIndex][wIndex - 1] == RSP_BLACK)     //���ϴ�Ϊ��ɫ
        {
          Short.Left = wIndex;                       //���Ϊ��˵�
        }
      }
      else                                           //���ϴ�Ϊ��ɫ
      {
        if(Img[hIndex][wIndex - 1] == RSP_WHITE)     //���ϴ�Ϊ��ɫ
        {
          Short.Right = wIndex - 1;                  //��¼Ϊ�Ҷ˵�
          if(wIndex - Short.Left <= RSP_vWL_MAX)  //�ж��߶��Ƿ�Ϊ����
          {
            RoadScan_RegionJudge(&RoadScan.Res, &Short, hIndex);
            if(RoadScan.Res.state == Exist)          //���ڣ�����β�ˣ��˳�˫for
            {
              if((abs(Res.x - RoadScan.Res.x) > 50 ||abs(Res.y - RoadScan.Res.y) > 50))
              {
                if(Res.state == Exist) RoadScan.Res = Res;
              }
              return;
            }
          }
          else                                       //�����ڣ�����ɨ��
          {
            Short.Right = 0;
            Short.Left = 0;
          }
        }
      }
    }
  }
}

/*
 *  @brief      ����ɨ�� ���������ж�
 *  @note       �жϸ����߶ε�����
 *  @param Data ��������
 *  @param Line �߶������и�
 *  @rtval      ���RoadResType����
 *  @date       2016.2.25
 */
void RoadScan_RegionJudge(RoadResType* Res, RoadSectionCellType* Data, uint8 Line)
{
  uint8  hIndex = 0, wIndex = 0;
  
  uint8  lWide  = Data->Left,  
         rWide  = Data->Right;
  uint8  lLimit = Data->Left, 
         rLimit = Data->Right,
         Location = (lLimit + rLimit) >> 1;
  uint32 xCoord = 0, yCoord = 0, Count  = 0;;
  
  Res->state = null;
  
  for(wIndex = Data->Left; wIndex < Data->Right; wIndex++) if(Img[Line + 1][wIndex] == RSP_WHITE) return;
  //��ɨ�跶Χ��ɨ���������׵㣩
  for(hIndex = Line; hIndex > RangeStartHigh; hIndex--)
  {
    Location = (lLimit + rLimit) >> 1;
    for(wIndex = Location; wIndex < RSP_WIDE - 1; wIndex++)
    {
      if(Img[hIndex][wIndex] == RSP_WHITE)      //ɨ�赽�׵�
      {
        Count++;
        xCoord += wIndex;
        yCoord += hIndex;
        if(wIndex - Location > RSP_vWL_MAX) return; //����������
      }
      else                                      //ɨ�赽�ڵ㣬ֹͣ����ɨ��
      {
        rLimit  = wIndex;// - 1;
        break;
      }
    }
    for(wIndex = Location; wIndex > 1; wIndex--)
    {
      if(Img[hIndex][wIndex] == RSP_WHITE)      //ɨ�赽�׵�
      {
        Count++;
        xCoord += wIndex;
        yCoord += hIndex;
        if(Location - wIndex > RSP_vWL_MAX) return; //����������
      }
      else                                      //ɨ�赽�ڵ㣬ֹͣ����ɨ��
      {
        lLimit  = wIndex;// + 1;
        break;
      }
    }
    if(hIndex - Line > RSP_vCL_MAX) return; //�߶�Խ��
    if(rLimit == lLimit) break;
    
    if(lWide > lLimit) {lWide = lLimit;}
    if(rWide < rLimit) {rWide = rLimit;}
  }
  if(hIndex == RangeStartHigh) return;
  if(Img[hIndex][Location] == RSP_WHITE) return;

  Res->x        = xCoord / Count;
  Res->y        = yCoord / Count;
  Res->Count    = Count;
  Res->High     = Line   - hIndex;
  Res->Wide     = rWide  - lWide - 2;
  
  Res->Points.xU = Res->x; Res->Points.yU = hIndex;
  Res->Points.xD = Res->x; Res->Points.yD = Line;
  Res->Points.xL = lWide;  Res->Points.yL = Res->y;
  Res->Points.xR = rWide;  Res->Points.yR = Res->y;
  if(Res->Count < RSP_vCL_MIN) return; //��Ŀ
  if(Res->High  < RSP_vHL_MIN) return; //�߶�
  if(Res->Wide  < RSP_vWL_MIN) return; //�߶�
  
  Res->state = Exist;
}


void RoadScan_RegionJudgePart(RoadResType* Res, uint8 xlocal, uint8 ylocal)
{
  uint8  hIndex = 0, wIndex = 0, hbuffer;
  uint8  lWide  = xlocal,  
         rWide  = xlocal;
  uint8  lLimit = xlocal, 
         rLimit = xlocal;
  uint32 xCoord = 0, yCoord = 0, Location = xlocal, Count = 0;
  Res->state = null;
  
  //��ɨ�跶Χ��ɨ���������׵㣩
  for(hIndex = ylocal; hIndex > RangeStartHigh; hIndex--)
  {
    Location = (lLimit + rLimit) >> 1;
    for(wIndex = Location; wIndex < RSP_WIDE - 1; wIndex++)
    { 
      if(Img[hIndex][wIndex] == RSP_WHITE)      //ɨ�赽�׵�
      {
        Count++;
        xCoord += wIndex;
        yCoord += hIndex;
        if(wIndex > Location + RSP_vWL_MAX) return; //����������
      }
      else                                      //ɨ�赽�ڵ㣬ֹͣ����ɨ��
      {
        rLimit  = wIndex;
        break;
      }
    }
    for(wIndex = Location; wIndex > 1; wIndex--)
    {
      if(Img[hIndex][wIndex] == RSP_WHITE)      //ɨ�赽�׵�
      {
        Count++;
        xCoord += wIndex;
        yCoord += hIndex;
        if(Location - wIndex > RSP_vWL_MAX) return; //����������
      }
      else                                      //ɨ�赽�ڵ㣬ֹͣ����ɨ��
      {
        lLimit  = wIndex;
        break;
      }
    }
    if(hIndex - ylocal > RSP_vCL_MAX) return; //�߶�Խ��
    if(rLimit == lLimit) break;
    lWide = (lWide > lLimit) ? lLimit : lWide;
    rWide = (rWide < rLimit) ? rLimit : rWide;
  }
  if(Img[hIndex][Location] == RSP_WHITE) return;
  hbuffer = hIndex;
  for(hIndex = ylocal; hIndex < RangeEndHigh; hIndex++)
  {
    Location = (lLimit + rLimit) >> 1;
    for(wIndex = Location; wIndex < RSP_WIDE - 1; wIndex++)
    { 
      if(Img[hIndex][wIndex] == RSP_WHITE)      //ɨ�赽�׵�
      {
        Count++;
        xCoord += wIndex;
        yCoord += hIndex;
        if(wIndex > Location + RSP_vWL_MAX) return; //����������
      }
      else                                      //ɨ�赽�ڵ㣬ֹͣ����ɨ��
      {
        rLimit  = wIndex;
        break;
      }
    }
    for(wIndex = Location; wIndex > 1; wIndex--)
    {
      if(Img[hIndex][wIndex] == RSP_WHITE)      //ɨ�赽�׵�
      {
        Count++;
        xCoord += wIndex;
        yCoord += hIndex;
        if(Location - wIndex > RSP_vWL_MAX) return; //����������
      }
      else                                      //ɨ�赽�ڵ㣬ֹͣ����ɨ��
      {
        lLimit  = wIndex;
        break;
      }
    }
    if(hIndex - ylocal > RSP_vCL_MAX) return; //�߶�Խ��
    if(rLimit == lLimit) break;
    lWide = (lWide > lLimit) ? lLimit : lWide;
    rWide = (rWide < rLimit) ? rLimit : rWide;
  }
  if(Img[hIndex][Location] == RSP_WHITE) return;
    
  Res->x        = xCoord / Count;
  Res->y        = yCoord / Count;
  Res->Count    = Count;
  Res->High     = hIndex - hbuffer;
  Res->Wide     = rWide  - lWide - 2;
  
  Res->Points.xU = Res->x; Res->Points.yU = hbuffer;
  Res->Points.xD = Res->x; Res->Points.yD = hIndex;
  Res->Points.xL = lWide;  Res->Points.yL = Res->y;
  Res->Points.xR = rWide;  Res->Points.yR = Res->y;
  
  if(Res->Count < RSP_vCL_MIN) return; //��Ŀ
  if(Res->High  < RSP_vHL_MIN) return; //�߶�
  if(Res->Wide  < RSP_vWL_MIN) return; //�߶�
  
  Res->state = Exist;
}

void RoadScan_Triangle(void)
{
  uint8 hIndex = 0, wIndex = 0, flag = 0;                     //�߶����������������ʼ��
  RoadScan.tRes.state = null;
  for(hIndex = RSP_HIGH - 1; hIndex > 0; hIndex--)
  {
      if(Img[hIndex][0] == RSP_WHITE && Img[hIndex][159] == RSP_WHITE)
      {
        for(wIndex = 1; wIndex < 79; wIndex++)
        {
          if(Img[hIndex][0 + wIndex] == RSP_BLACK || Img[hIndex][159 - wIndex] == RSP_BLACK)
          {
            if(flag == 0) return;
            if(wIndex < RSP_TRI_OFFSET) break;
            else RoadScan_TriPoint(&RoadScan.pRes, hIndex, wIndex);
            if(RoadScan.pRes.state) 
              return;
          }
        }
      }
      if(wIndex == 79) flag = 1;
  }
}

void RoadScan_TriPoint(RoadTriPoints* pRes, uint8 Line, uint8 index)
{
  uint8 wIndex, hIndex = Line;
  uint8 lOffset,lh, rOffset,rh;
  uint8 flag = 0;
  if(Img[Line][index] == RSP_WHITE) index = 159 - index;
  pRes->x1 = index; pRes->y1 = Line;
  wIndex = lOffset = rOffset = index;
  for(hIndex = Line; hIndex > RSP_TRI_HLIMIT; hIndex--)
  {
    for(wIndex = RSP_TRI_OFFSET; wIndex < RSP_TRI_OFFSET2; wIndex++)
    {
      if(Img[hIndex][wIndex] == RSP_WHITE)
      { 
        if(Img[hIndex][wIndex + 1] == RSP_BLACK)
        {
          if(lOffset > wIndex) {lOffset = wIndex + 1; lh      = hIndex;}
          flag = 1;
        }
      }
      else if(Img[hIndex][wIndex + 1] == RSP_WHITE)
      {
        if(rOffset < wIndex){ rOffset = wIndex; rh = hIndex;}
        pRes->y3 = hIndex;
        pRes->x3 = wIndex;
        break;
      }
    }
    if(flag) flag = 0; else break;
  }
  if(Line - hIndex > 9) pRes->state = Exist;
  else return;
  
  if(abs(lh - rh) < 4)
  {
    pRes->y2 = lh;
    if(pRes->x1 == rOffset) pRes->x2 = lOffset;
    else pRes->x2 = rOffset;
    pRes->state = Tri_Front;
  }
  else
  {
    if(abs(lOffset - pRes->x3) > abs(rOffset - pRes->x3))
    {
      pRes->x2 = lOffset;
      pRes->y2 = lh;
      pRes->state = Tri_Left;
    }
    else
    {
      pRes->x2 = rOffset;
      pRes->y2 = rh;
      pRes->state = Tri_Right;
    }
  }
  pRes->x = (pRes->x1 + pRes->x2 + pRes->x3) / 3;
  pRes->y = (pRes->y1 + pRes->y2 + pRes->y3) / 3;
}

void RoadScan_StartLine(void)
{
  uint8 hIndex = 0, wIndex = 0;                 //�߶����������������ʼ��
  RoadSectionCellType Short_L = {0,0,none}, Short_R = {0,0,none};
  
  for(hIndex = RangeEndHigh; hIndex > 60; hIndex--)
  {
    Short_R.Right = Short_R.Left  = 0;
    for(wIndex = 81; wIndex < RSP_WIDE - 1; wIndex++)
    {
      if(Img[hIndex][wIndex] == RSP_BLACK)
      {
        if(Img[hIndex][wIndex - 1] == RSP_WHITE) Short_R.Left = wIndex;
      }
      else
      {
        if(Img[hIndex][wIndex - 1] == RSP_BLACK)
        {
          Short_R.Right = wIndex - 1;
          RoadScan_StartRegionJudge(&RoadScan.sRes, &Short_R, hIndex);
          if(RoadScan.sRes.state == Exist_Right) hIndex = RangeStartHigh;
          else Short_R.Right = Short_R.Left = 0;
        }
      }
    }
  }
  if(RoadScan.sRes.state == null) return;
  for(hIndex = RangeEndHigh; hIndex > 60; hIndex--)
  {
    Short_L.Right = Short_L.Left  = 0;
    for(wIndex = 80; wIndex > 1; wIndex--)
    {
      if(Img[hIndex][wIndex] == RSP_BLACK)
      {
        if(Img[hIndex][wIndex + 1] == RSP_WHITE) Short_L.Right = wIndex;
      }
      else if(Img[hIndex][wIndex + 1] == RSP_BLACK)
      {
        Short_L.Left  = wIndex - 1;
        RoadScan_StartRegionJudge2(&RoadScan.sRes, &Short_L, hIndex);
        if(RoadScan.sRes.state == Exist) return;
        else Short_L.Right = Short_L.Left  = 0;
      }
    }
  }
}
void RoadScan_StartRegionJudge(RoadStartType* Res, RoadSectionCellType* Data, uint8 Line)
{
  uint8  hIndex = 0, wIndex = 0;
  uint8  lWide  = Data->Left, rWide  = Data->Right;
  uint8  lLimit = Data->Left, rLimit = Data->Right, Location = (lLimit + rLimit) >> 1;
  uint32 xCoord = 0, yCoord = 0, Count = 0;
  Res->state = null;
  
  //��ɨ�跶Χ��ɨ���������׵㣩
  for(hIndex = Line; hIndex > RangeStartHigh; hIndex--)
  {
    Location = (lLimit + rLimit) >> 1;
    for(wIndex = Location; wIndex < RSP_WIDE - 1; wIndex++)
    {
      if(Img[hIndex][wIndex] == RSP_BLACK)
      {
        Count++;
        xCoord += wIndex;
        yCoord += hIndex;
      }
      else                                      //ɨ�赽�ڵ㣬ֹͣ����ɨ��
      {
        rLimit  = wIndex;
        break;
      }
    }
    for(wIndex = Location; wIndex > 1; wIndex--)
    {
      if(Img[hIndex][wIndex] == RSP_BLACK)      //ɨ�赽�׵�
      {
        Count++;
        xCoord += wIndex;
        yCoord += hIndex;
      }
      else                                      //ɨ�赽�ڵ㣬ֹͣ����ɨ��
      {
        lLimit  = wIndex;
        break;
      }
    }
    if(rLimit == lLimit) break;
    if(lWide > lLimit) {lWide = lLimit;}
    if(rWide < rLimit) {rWide = rLimit;}
  }

  Res->xRight   = xCoord / Count;
  Res->yRight   = yCoord / Count;
  Res->hRight   = Line   - hIndex;
  Res->wRight   = rWide  - lWide - 2;
  if(Res->hRight > 5) return;
  Res->state = Exist_Right;
}

void RoadScan_StartRegionJudge2(RoadStartType* Res, RoadSectionCellType* Data, uint8 Line)
{
  uint8  hIndex = 0, wIndex = 0, lWide  = Data->Left, rWide  = Data->Right, lLimit = Data->Left, rLimit = Data->Right, Location = (lLimit + rLimit) >> 1;
  uint32 xCoord = 0, yCoord = 0, Count = 0;
  Res->state = null;
  
  //��ɨ�跶Χ��ɨ���������׵㣩
  for(hIndex = Line; hIndex > RangeStartHigh; hIndex--)
  {
    Location = (lLimit + rLimit) >> 1;
    for(wIndex = Location; wIndex < RSP_WIDE - 1; wIndex++)
    {
      if(Img[hIndex][wIndex] == RSP_BLACK)
      {
        Count++;
        xCoord += wIndex;
        yCoord += hIndex;
      }
      else                                      //ɨ�赽�ڵ㣬ֹͣ����ɨ��
      {
        rLimit  = wIndex;
        break;
      }
    }
    for(wIndex = Location; wIndex > 1; wIndex--)
    {
      if(Img[hIndex][wIndex] == RSP_BLACK)      //ɨ�赽�׵�
      {
        Count++;
        xCoord += wIndex;
        yCoord += hIndex;
      }
      else                                      //ɨ�赽�ڵ㣬ֹͣ����ɨ��
      {
        lLimit  = wIndex;
        break;
      }
    }
    if(rLimit == lLimit) break;
    
    if(lWide > lLimit) {lWide = lLimit;}
    if(rWide < rLimit) {rWide = rLimit;}
  }

  Res->xLeft   = xCoord / Count;
  Res->yLeft   = yCoord / Count;
  Res->hLeft   = Line   - hIndex;
  Res->wLeft   = rWide  - lWide - 2;
  if(Res->hLeft > 5) return;
  Res->state = Exist;
}
