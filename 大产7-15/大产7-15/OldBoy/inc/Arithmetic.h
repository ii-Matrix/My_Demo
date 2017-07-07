/*
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2016,���к�
 *     All rights reserved.
 *
 * @file       Arithmetic.c
 * @brief      ǰ���жϺ�����
 * @author     ���к�����С˼ͬѧ
 * @version    v1.2
 * @date       2016.2.25
***************************************************
 * ��ֲ˵����
 *  0.�� Arithmetic.h ͷ�ļ�����
 *  1.��main.c ���������  extern RoadScanType RoadScan;
 *  2.�ڳ�ʼ����ʱ�����   RoadScan_Init(); 
 *  3.�ڵ��õ�ʱ��ʹ��     RoadScan.Plan.Plan1();
      ɨ�������Զ�װ��RoadScan�ṹ����
 *  4.RoadScan.Res�д�����״̬�ͽ��
 */

#ifndef __ARITHMETIC_H__
#define __ARITHMETIC_H__

#include "common.h"

#define RSP_BLACK   0x7E        //��ɫ
#define RSP_WHITE   0x21        //��ɫ
#define RSP_HIGH     120        //ͼ��߶�
#define RSP_WIDE     160        //ͼ����
#define RSP_CLIMIT    16        //�����ƣ����ֵ��
#define RSP_LLIMIT     4        //�����ƣ���Сֵ��
#define RSP_WLIMIT     4        //�������
#define RSP_HLIMIT     3        //�߶�����

#define RSP_CLIMIT_MAX   200        //�����ƣ����ֵ��
#define RSP_CLIMIT_MIN    15        //�����ƣ���Сֵ��
#define RSP_WLIMIT_MAX    20        //������ƣ����ֵ��
#define RSP_WLIMIT_MIN     4        //������ƣ���Сֵ��
#define RSP_HLIMIT_MAX    15        //�߶����ƣ����ֵ��
#define RSP_HLIMIT_MIN     3        //�߶����ƣ���Сֵ��

//  uint8 RangeStartHigh =   30     + RSP_HLIMIT_MAX, //�ϱ�Ԥ��һ����������߶�
//        RangeStartWide =            RSP_WLIMIT_MAX, //���Ԥ��һ������������
//        RangeEndHigh   = RSP_HIGH - RSP_HLIMIT_MAX, //�±�Ԥ��һ����������߶�
//        RangeEndWide   = RSP_WIDE - RSP_WLIMIT_MAX; //�ұ�Ԥ��һ������������

#define RangeStartHigh     35//30       + RSP_HLIMIT_MAX
#define RangeStartWide     20//RSP_WLIMIT_MAX
#define RangeEndHigh      119//RSP_HIGH - RSP_HLIMIT_MAX
#define RangeEndWide      140//RSP_WIDE - RSP_WLIMIT_MAX

#define RSP_TRI_OFFSET    45         //�����ε�ĳ�߾���������Сֵ
#define RSP_TRI_OFFSET2  114         //�����ε�ĳ�߾���������һ��
#define RSP_TRI_HLIMIT    40         //���޽�


#define ImgBitBand(var_b, bit_b)  (*(uint32_t volatile*)(0x22000000 + 32*((uint32_t)&var_b - 0x20000000) +  4*(bit_b)))
#define ImgColorBit(value, index) (((value >> 7)? 7:(value >> 6)?6:(value>>5)? 5:(value >> 4)? 4:(value >> 3)? 3:(value >> 2)? 2:(value >> 1)? 1:0) + (index << 3))
/*
 * @brief ����ɨ�衪��״̬ö������
 * @param null  ��  ����ǰ��û�г�
 * @param Exist ���ڣ���ǰ���г�
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
 * @brief ����ɨ�衪������ö������
 * @param none  ��                ������Ч��
 * @param ubwb  δ��ȡ�ڰ׺�����������Ŀ������
 */
typedef enum
{
  none,
  ubwb,
  wbw,
}RoadSectionTypeEnum;

/*
 * @brief ����ɨ�衪���������鵥Ԫ�ṹ��
 * @note  ����ɨ��ʱ�����ͣ����ڴ��������������͡�
 * @param Left  ������Ԫ��߽�
 * @param Right ������Ԫ��߽�
 * @param state ������Ԫ���ͣ����RoadSectionTypeEnumö������
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
 * @brief ����ɨ�衪������ɨ�����ݵ�Ԫ�ṹ��
 * @note  �洢ǰ��������ͼ�ε�����
 * @param x     ǰ������ͼ�εĺ�����
 * @param y     ǰ������ͼ�ε�������
 * @param High  ǰ������ͼ�εĸ߶�
 * @param Wide  ǰ������ͼ�εĿ��
 * @param Count ǰ������ͼ�εİ׵����
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
 * @brief ����ɨ�衪������ɨ���ܽṹ��
 * @note  �й�������ɨ���ȫ��
 * @param Plan  ��������չѡ������Ժ�������������RoadPlanType����
 * @param Res   �洢ɨ���������RoadResType����
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
typedef struct
{
  uint8 state;
  uint32 Time;
  uint32 Start_Time;
  uint32 End_Time;
  float Res;
}SonicType;
#define SONIC_DATA  PTD8
#define SONIC_STATE PTD9
#define SONIC_CHECK gpio_get(SONIC_STATE)
#define SONIC_RIGHT 1
#define SONIC_WRONG 0

extern SonicType Sonic;

extern RoadScanType RoadScan;
void RoadScan_Init(void);       //����ɨ���ʼ��
void RoadScan_Plan2(void);      //����ɨ�衪������2
void RoadScan_Triangle(void);
void RoadScan_TriPoint(RoadTriPoints* pRes, uint8 Line, uint8 index);
void RoadScan_RegionJudge(RoadResType* Res, RoadSectionCellType* Data, uint8 Line);
void RoadScan_RegionJudgePart(RoadResType* Res, uint8 wIndex, uint8 hIndex);
void RoadScan_TriJudge(RoadTriResType* tRes, uint8 Line, uint8 index);
void RoadScan_StartLine(void);
void RoadScan_StartRegionJudge(RoadStartType* Res, RoadSectionCellType* Data, uint8 Line);
void RoadScan_StartRegionJudge2(RoadStartType* Res, RoadSectionCellType* Data, uint8 Line);

void Sonic_Init(void);
void Sonic_Calc(void);

#endif
