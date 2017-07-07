#ifndef _OLDBOY_DMA_H_
#define _OLDBOY_DMA_H_
#include "stdbool.h"
//触发模式选择
typedef enum
{
    kDMA_TriggerSource_Normal,    //正常触发
    kDMA_TriggerSource_Periodic,  //周期触发
}DMA_TriggerSource_Type;
// DMA搬运端口位宽设置
typedef enum
{
    kDMA_DataWidthBit_8,   /* 8 bit data width */
    kDMA_DataWidthBit_16,  /* 16 bit data width */
    kDMA_DataWidthBit_32,  /* 32 bit data width*/
}DMA_DataWidthBit_Type; 

// DMA Moduluo 设置
typedef enum 
{
    kDMA_ModuloDisable = 0x0U,
    kDMA_Modulo2bytes = 0x1U,
    kDMA_Modulo4bytes = 0x2U,
    kDMA_Modulo8bytes = 0x3U,
    kDMA_Modulo16bytes = 0x4U,
    kDMA_Modulo32bytes = 0x5U,
    kDMA_Modulo64bytes = 0x6U,
    kDMA_Modulo128bytes = 0x7U,
    kDMA_Modulo256bytes = 0x8U,
    kDMA_Modulo512bytes = 0x9U,
    kDMA_Modulo1Kbytes = 0xaU,
    kDMA_Modulo2Kbytes = 0xbU,
    kDMA_Modulo4Kbytes = 0xcU,
    kDMA_Modulo8Kbytes = 0xdU,
    kDMA_Modulo16Kbytes = 0xeU,
    kDMA_Modulo32Kbytes = 0xfU,
    kDMA_Modulo64Kbytes = 0x10U,
    kDMA_Modulo128Kbytes = 0x11U,
    kDMA_Modulo256Kbytes = 0x12U,
    kDMA_Modulo512Kbytes = 0x13U,
    kDMA_Modulo1Mbytes = 0x14U,
    kDMA_Modulo2Mbytes = 0x15U,
    kDMA_Modulo4Mbytes = 0x16U,
    kDMA_Modulo8Mbytes = 0x17U,
    kDMA_Modulo16Mbytes = 0x18U,
    kDMA_Modulo32Mbytes = 0x19U,
    kDMA_Modulo64Mbytes = 0x1aU,
    kDMA_Modulo128Mbytes = 0x1bU,
    kDMA_Modulo256Mbytes = 0x1cU,
    kDMA_Modulo512Mbytes = 0x1dU,
    kDMA_Modulo1Gbytes = 0x1eU,
    kDMA_Modulo2Gbytes = 0x1fU
} DMA_Modulo_Type;
typedef enum
{
    DADDR_Recover = 0,        //恢复目的地址
    DADDR_KeepOn  = 1,        //目的地址保持不变
} DMA_CFG;
// 初始化结构体
typedef struct 
{
    uint8                     ch;                                /* DMA通道号0~15 */       
    uint8                     chlTriggerSource;                   /* DMA触发源选择 */
    uint16                    minorLoopByteCnt;                   /* MINOR LOOP 中一次传输的字节数 */
    uint16                    majorLoopCnt;                       /* MAJOR LOOP 循环次数 */
    DMA_TriggerSource_Type      triggerSourceMode;                  /* 触发模式选择 */
    /* 源地址配置 */
    int32                     sAddrOffset;                /* DMA每次读取sAddr之后 sAddr的偏移量 可正可负 */
    uint32                    sAddr;                      /* 数据源地址 */
    DMA_DataWidthBit_Type       sDataWidth;                 /* 数据源地址数据宽度 8 16 32 */
    int32                     sLastAddrAdj;               /* 所有MAJOR LOOP循环完成后 源地址偏移量 */
    DMA_Modulo_Type             sMod;                       /* Modulo 设置 参见 AN2898 */
    /* 目标地址属性配置 */
    int32                     dAddrOffset;                
    uint32                    dAddr;                      
    DMA_DataWidthBit_Type       dDataWidth;                 
    int32                     dLastAddrAdj;               
    DMA_Modulo_Type             dMod;   
    
    DMA_CFG                     cfg;
    
}DMA_InitTypeDef;
//中断及DMA配置
typedef enum
{
    kUART_IT_Tx,                // 开启每发送一帧完成中断 
    kUART_DMA_Tx,               // 开启每发送一帧触发DMA
    kUART_IT_Rx,                // 开启每接收一帧完成中断 
    kUART_DMA_Rx,               // 开启每接收一帧完成中断 
}UART_ITDMAConfig_Type;



void DMA_UART_Send_Init(UARTn_e uartInstnace,uint32 baud,DMA_InitTypeDef *DMA_InitStruct);
void DMA_Init(DMA_InitTypeDef *DMA_InitStruct);
void UART_ITDMAConfig(UARTn_e uartn, UART_ITDMAConfig_Type config, bool status);
void DMA_SetSourceAddress(uint8 ch, uint32 address);
uint32 DMA_GetSourceAddress(uint8 ch);
void UART_ITDMAConfig(UARTn_e uartn, UART_ITDMAConfig_Type config, bool status);
void DMA_SetMajorLoopCounter(uint8 ch, uint32 val);
void UART_SendWithDMA(DMA_InitTypeDef *DMA_InitStruct, uint8 *buf,uint32 length);
void DMA_UART_Repeat(DMA_InitTypeDef *DMA_InitStruct);
uint8 DMA_IsMajorLoopComplete(DMA_InitTypeDef *DMA_InitStruct);
#endif 