/*
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2016,老男孩
 *     All rights reserved.
 *
 * @file       Arithmetic.c
 * @brief      前车判断函数库
 * @author     老男孩——小思同学
 * @version    v1.2
 * @date       2016.2.25
***************************************************
 * 移植说明：
 *  0.将 Arithmetic.h 头文件包含
 *  1.在main.c 里加入声明  extern RoadScanType RoadScan;
 *  2.在初始化的时候加入   RoadScan_Init(); 
 *  3.在调用的时候使用     RoadScan.Plan.Plan1();
      扫描结果将自动装入RoadScan结构体中
 *  4.RoadScan.Res中储存了状态和结果
 */

#ifndef __ARITHMETIC_H__
#define __ARITHMETIC_H__

#include "common.h"

#define RSP_BLACK   0x7E        //黑色
#define RSP_WHITE   0x21        //白色
#define RSP_HIGH     120        //图像高度
#define RSP_WIDE     160        //图像宽度
#define RSP_CLIMIT    16        //点限制（最大值）
#define RSP_LLIMIT     4        //点限制（最小值）
#define RSP_WLIMIT     4        //宽度限制
#define RSP_HLIMIT     3        //高度限制

#define RSP_CLIMIT_MAX   150        //点限制（最大值）
#define RSP_CLIMIT_MIN    15        //点限制（最小值）
#define RSP_WLIMIT_MAX    20        //宽度限制（最大值）
#define RSP_WLIMIT_MIN     4        //宽度限制（最小值）
#define RSP_HLIMIT_MAX    15        //高度限制（最大值）
#define RSP_HLIMIT_MIN     3        //高度限制（最小值）

//  uint8 RangeStartHigh =   30     + RSP_HLIMIT_MAX, //上边预留一个最大特征高度
//        RangeStartWide =            RSP_WLIMIT_MAX, //左边预留一个最大特征宽度
//        RangeEndHigh   = RSP_HIGH - RSP_HLIMIT_MAX, //下边预留一个最大特征高度
//        RangeEndWide   = RSP_WIDE - RSP_WLIMIT_MAX; //右边预留一个最大特征宽度

#define RangeStartHigh     35//30       + RSP_HLIMIT_MAX
#define RangeStartWide     20//RSP_WLIMIT_MAX
#define RangeEndHigh      119//RSP_HIGH - RSP_HLIMIT_MAX
#define RangeEndWide      140//RSP_WIDE - RSP_WLIMIT_MAX

#define RSP_TRI_OFFSET    45         //三角形的某边距赛道的最小值
#define RSP_TRI_OFFSET2  114         //三角形的某边距赛道的另一侧
#define RSP_TRI_HLIMIT    40         //上限界
/*
 * @brief 赛道扫描——状态枚举类型
 * @param null  空  ，即前方没有车
 * @param Exist 存在，即前方有车
 */
typedef enum
{
  null,
  Exist,
  Exist_Judge,
  Exist_Left,
  Exist_Right,
  Tri_Left,
  Tri_Right,
  Tri_Front,
}RoadScanStateEnum;

/*
 * @brief 赛道扫描——赛道枚举类型
 * @param none  空                ，即无效项
 * @param ubwb  未读取黑白黑型赛道，即目标类型
 */
typedef enum
{
  none,
  ubwb,
}RoadSectionTypeEnum;

/*
 * @brief 赛道扫描——赛道分组单元结构体
 * @note  横向扫描时用类型，用于储存特殊赛道类型。
 * @param Left  特征单元左边界
 * @param Right 特征单元左边界
 * @param state 特征单元类型，详见RoadSectionTypeEnum枚举类型
 */
typedef struct
{
  uint8 Left;
  uint8 Right;
  RoadSectionTypeEnum state; 
}
RoadSectionCellType;

typedef struct 
{
  uint8 xU,yU;
  uint8 xD,yD;
  uint8 xL,yL;
  uint8 xR,yR;
}RoadResFourPointType;

/*
 * @brief 赛道扫描——赛道扫描数据单元结构体
 * @note  存储前车的特征图形的数据
 * @param x     前车特征图形的横坐标
 * @param y     前车特征图形的纵坐标
 * @param High  前车特征图形的高度
 * @param Wide  前车特征图形的宽度
 * @param Count 前车特征图形的白点个数
 */
typedef struct 
{
  uint8 x;
  uint8 y;
  uint8 High;
  uint8 Wide;
  uint32 Count;
  RoadScanStateEnum state;
  RoadResFourPointType Points;
}RoadResType;



typedef struct 
{
  uint8 x;
  uint8 y;
  uint8 High;
  uint8 Wide;
  RoadScanStateEnum state;
}RoadTriResType;

typedef struct 
{
  uint8 x1, y1;
  uint8 x2, y2;
  uint8 x3, y3;
  uint8 x, y;
  RoadScanStateEnum state;
}RoadTriPoints;

typedef struct 
{
  uint8 xLeft, yLeft;
  uint8 wLeft, hLeft;
  uint8 xRight, yRight;
  uint8 wRight, hRight;
  RoadScanStateEnum state;
}RoadStartType;


/*
 * @brief 赛道扫描——赛道扫描总结构体
 * @note  有关于赛道扫描的全部
 * @param Plan  可视作扩展选项，方便以后更换方案，详见RoadPlanType类型
 * @param Res   存储扫描结果，详见RoadResType类型
 */
typedef struct
{
  void (*Plan)(void);
  void (*PlanTri)(void);
  void (*TriPoint)(void);
  void (*PlanStart)(void);
  RoadResType       Res;
  RoadTriResType   tRes;
  RoadTriPoints    pRes;
  RoadStartType    sRes;
}RoadScanType;

extern RoadScanType RoadScan;
void RoadScan_Init(void);       //赛道扫描初始化
void RoadScan_Plan2(void);      //赛道扫描——方案2
void RoadScan_Triangle(void);
void RoadScan_TriPoint(RoadTriPoints* pRes, uint8 Line, uint8 index);
void RoadScan_RegionJudge(RoadResType* Res, RoadSectionCellType* Data, uint8 Line);
void RoadScan_RegionJudgePart(RoadResType* Res, uint8 wIndex, uint8 hIndex);
void RoadScan_TriJudge(RoadTriResType* tRes, uint8 Line, uint8 index);
void RoadScan_StartLine(void);
void RoadScan_StartRegionJudge(RoadStartType* Res, RoadSectionCellType* Data, uint8 Line);
void RoadScan_StartRegionJudge2(RoadStartType* Res, RoadSectionCellType* Data, uint8 Line);
#endif
