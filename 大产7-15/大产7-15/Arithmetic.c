/*
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2016,老男孩
 *     All rights reserved.
 *
 * @file       Arithmetic.c
 * @brief      前车判断函数库
 * @author     小思同学 老男孩
 * @version    v1.3 增加方案2
 * @date       2016.2.27
***************************************************
 * 移植说明：
 *  0.将 Arithmetic.h 头文件包含
 *  1.在main.c 里加入声明  extern RoadScanType RoadScan;
 *  2.在初始化的时候加入   RoadScan_Init(); 
 *  3.在调用的时候使用     RoadScan.Plan.Plan1();
      扫描结果将自动装入RoadScan结构体中
 *  4.RoadScan.Res中储存了状态和结果
 */

#include "common.h"
#include "include.h"
#include "Arithmetic.h"

extern uint8 Img[CAMERA_H][CAMERA_W];           //解压缩后的图像
RoadScanType RoadScan;                          //赛道扫描结构体初始化

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
 *  @brief      赛道扫描初始化函数 ver1.0
 *  @note       赛道扫描结构体，含有方案定向、过程以及结果的初始化。
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
 *  @brief      赛道扫描 方案一 ver1.0
 *  @note       寻找封闭，且封闭图形符合一定规则的区域并定位
 *  @date       2016.2.27
 */
void RoadScan_Plan2(void)
{
  static uint8_t BreakOffsetX = 0, BreakOffsetY = 0;
  static int8_t OffsetW = 0, OffsetH = 0;
  static RoadResType Res;
  static RoadResType ResKeep;
  uint8 hIndex = 0, wIndex = 0;                 //高度索引及宽度索引初始化
  RoadSectionCellType Short = {0,0,none};       //分组特征单元初始化
  
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
  
  
  //从左上方开始扫描，寻找白线
  if(RoadScan.Res.state == Exist) Res = RoadScan.Res;
  if(Res.state == Exist)
  {
    RoadScan_RegionJudgePart(&RoadScan.Res, Res.x, Res.y);
    if(RoadScan.Res.state == Exist)          //存在，跳至尾端，退出双for
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
    Short.Right = 0;                            //分组单元结构体初始化，每次需要
    Short.Left  = 0;
    for(wIndex = 0; wIndex < RSP_WIDE; wIndex++)
    {
      if(Img[hIndex][wIndex] == RSP_WHITE)           //寻找到白色
      {
        if(Img[hIndex][wIndex - 1] == RSP_BLACK)     //且上次为黑色
        {
          Short.Left = wIndex;                       //标记为左端点
        }
      }
      else                                           //且上次为黑色
      {
        if(Img[hIndex][wIndex - 1] == RSP_WHITE)     //且上次为白色
        {
          Short.Right = wIndex - 1;                  //记录为右端点
          if(wIndex - Short.Left <= RSP_vWL_MAX)  //判断线段是否为短线
          {
            RoadScan_RegionJudge(&RoadScan.Res, &Short, hIndex);
            if(RoadScan.Res.state == Exist)          //存在，跳至尾端，退出双for
            {
              if((abs(Res.x - RoadScan.Res.x) > 50 ||abs(Res.y - RoadScan.Res.y) > 50))
              {
                if(Res.state == Exist) RoadScan.Res = Res;
              }
              return;
            }
          }
          else                                       //不存在，继续扫描
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
 *  @brief      赛道扫描 特征区域判断
 *  @note       判断给定线段的类型
 *  @param Data 给定数据
 *  @param Line 线段所在行高
 *  @rtval      详见RoadResType类型
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
  //在扫描范围内扫描特征（白点）
  for(hIndex = Line; hIndex > RangeStartHigh; hIndex--)
  {
    Location = (lLimit + rLimit) >> 1;
    for(wIndex = Location; wIndex < RSP_WIDE - 1; wIndex++)
    {
      if(Img[hIndex][wIndex] == RSP_WHITE)      //扫描到白点
      {
        Count++;
        xCoord += wIndex;
        yCoord += hIndex;
        if(wIndex - Location > RSP_vWL_MAX) return; //过长，超界
      }
      else                                      //扫描到黑点，停止本次扫描
      {
        rLimit  = wIndex;// - 1;
        break;
      }
    }
    for(wIndex = Location; wIndex > 1; wIndex--)
    {
      if(Img[hIndex][wIndex] == RSP_WHITE)      //扫描到白点
      {
        Count++;
        xCoord += wIndex;
        yCoord += hIndex;
        if(Location - wIndex > RSP_vWL_MAX) return; //过长，超界
      }
      else                                      //扫描到黑点，停止本次扫描
      {
        lLimit  = wIndex;// + 1;
        break;
      }
    }
    if(hIndex - Line > RSP_vCL_MAX) return; //高度越界
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
  if(Res->Count < RSP_vCL_MIN) return; //数目
  if(Res->High  < RSP_vHL_MIN) return; //高度
  if(Res->Wide  < RSP_vWL_MIN) return; //高度
  
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
  
  //在扫描范围内扫描特征（白点）
  for(hIndex = ylocal; hIndex > RangeStartHigh; hIndex--)
  {
    Location = (lLimit + rLimit) >> 1;
    for(wIndex = Location; wIndex < RSP_WIDE - 1; wIndex++)
    { 
      if(Img[hIndex][wIndex] == RSP_WHITE)      //扫描到白点
      {
        Count++;
        xCoord += wIndex;
        yCoord += hIndex;
        if(wIndex > Location + RSP_vWL_MAX) return; //过长，超界
      }
      else                                      //扫描到黑点，停止本次扫描
      {
        rLimit  = wIndex;
        break;
      }
    }
    for(wIndex = Location; wIndex > 1; wIndex--)
    {
      if(Img[hIndex][wIndex] == RSP_WHITE)      //扫描到白点
      {
        Count++;
        xCoord += wIndex;
        yCoord += hIndex;
        if(Location - wIndex > RSP_vWL_MAX) return; //过长，超界
      }
      else                                      //扫描到黑点，停止本次扫描
      {
        lLimit  = wIndex;
        break;
      }
    }
    if(hIndex - ylocal > RSP_vCL_MAX) return; //高度越界
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
      if(Img[hIndex][wIndex] == RSP_WHITE)      //扫描到白点
      {
        Count++;
        xCoord += wIndex;
        yCoord += hIndex;
        if(wIndex > Location + RSP_vWL_MAX) return; //过长，超界
      }
      else                                      //扫描到黑点，停止本次扫描
      {
        rLimit  = wIndex;
        break;
      }
    }
    for(wIndex = Location; wIndex > 1; wIndex--)
    {
      if(Img[hIndex][wIndex] == RSP_WHITE)      //扫描到白点
      {
        Count++;
        xCoord += wIndex;
        yCoord += hIndex;
        if(Location - wIndex > RSP_vWL_MAX) return; //过长，超界
      }
      else                                      //扫描到黑点，停止本次扫描
      {
        lLimit  = wIndex;
        break;
      }
    }
    if(hIndex - ylocal > RSP_vCL_MAX) return; //高度越界
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
  
  if(Res->Count < RSP_vCL_MIN) return; //数目
  if(Res->High  < RSP_vHL_MIN) return; //高度
  if(Res->Wide  < RSP_vWL_MIN) return; //高度
  
  Res->state = Exist;
}

void RoadScan_Triangle(void)
{
  uint8 hIndex = 0, wIndex = 0, flag = 0;                     //高度索引及宽度索引初始化
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
  uint8 hIndex = 0, wIndex = 0;                 //高度索引及宽度索引初始化
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
  
  //在扫描范围内扫描特征（白点）
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
      else                                      //扫描到黑点，停止本次扫描
      {
        rLimit  = wIndex;
        break;
      }
    }
    for(wIndex = Location; wIndex > 1; wIndex--)
    {
      if(Img[hIndex][wIndex] == RSP_BLACK)      //扫描到白点
      {
        Count++;
        xCoord += wIndex;
        yCoord += hIndex;
      }
      else                                      //扫描到黑点，停止本次扫描
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
  
  //在扫描范围内扫描特征（白点）
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
      else                                      //扫描到黑点，停止本次扫描
      {
        rLimit  = wIndex;
        break;
      }
    }
    for(wIndex = Location; wIndex > 1; wIndex--)
    {
      if(Img[hIndex][wIndex] == RSP_BLACK)      //扫描到白点
      {
        Count++;
        xCoord += wIndex;
        yCoord += hIndex;
      }
      else                                      //扫描到黑点，停止本次扫描
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
